/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "memlogd_main.h"
#include "memlogAgent.h"

std::list<MemlogObject *> objList;

bool memlogger_init(void)
{
    ALOGE("%s", __func__);
    return 0;
}

void ThreadLoop(void)
{
    if (objList.size()) {
        for (auto iter = objList.begin(); iter != objList.end();) {
            int n = 0;
            auto obj = *iter;
            if (obj == nullptr) {
                iter = objList.erase(iter);
                continue;
            }
            if (obj->paused) {
                iter++;
                continue;
            }
            dirent **filelist;
            n = scandir(obj->treeName.c_str(), &filelist, NULL, NULL);

            if (n <= 0) {
                ALOGI("free file object %s", obj->fileName.c_str());
                close(obj->inFd);
                iter = objList.erase(iter);
                delete(obj);
                continue;
            }

            for (int i = 0 ; i < n ; i++)
                free(filelist[i]);
            free(filelist);

            int64_t ret = obj->write();
            if (ret == -POLLERR) {
                free_file(obj);
            } else if (ret > 0)
                ALOGI("%s: saved %ld bytes at %s", __func__, ret, obj->getFileName());
            iter++;
        }
    }
}

int create_file(memlog_file_cmd *cmd)
{
    char path[80];
    int n;
    int retry_cnt = 3;
    dirent **namelist;
    MemlogObject *obj = new MemlogObject();

    if (!obj) {
        ALOGE("%s: NULL obj", __func__);
        return -1;
    }
    std::string path_tree = PATH_REQ;
    obj->fileName = cmd->file_name;
    obj->descName = cmd->desc_name;

    obj->maxFileCnt = cmd->max_file_num;
    obj->maxFileSize = cmd->max_file_size + PADDING_SIZE;

    umask(000);

    std::regex header("memlog-\\d*\\-");
    obj->fileName = std::regex_replace(obj->fileName, header, "");

    path_tree = path_tree + obj->descName + "/";
    //ALOGI("%s tree %s", __func__, path_tree.c_str());
    n = scandir(path_tree.c_str(), &namelist, NULL, alphasort);
    if (n > 0) {
        int i = n;
        while(i--) {
            int fd, ret;
            char buf_name[50];
            memset(buf_name, 0, sizeof(buf_name));
            if (!strncmp(namelist[i]->d_name, ".", 1))
                continue;
            fd = open((path_tree + namelist[i]->d_name + "/name").c_str(), O_RDONLY);
            if (fd < 0)
                ALOGE("%s fail %s", (path_tree + namelist[i]->d_name + "/name").c_str(), strerror(errno));
            else {
                ret = read(fd, buf_name, sizeof(buf_name));
                close(fd);
                if (ret < 0) {
                    ALOGE("%s read fail", __func__);
                    continue;
                }
                //ALOGI("%s buf (%zu)%s", __func__, strlen(buf_name), buf_name);
                buf_name[strlen(buf_name)-1] = 0;
                if (!strcmp(buf_name, obj->fileName.c_str())) {
                    obj->treeName = path_tree + namelist[i]->d_name;
                    ALOGI("%s tree found at %s", __func__, obj->treeName.c_str());
                    break;
                }
            }
        }
        for (int i = 0 ; i < n ; i++)
            free(namelist[i]);
        free(namelist);
    }

    sprintf(path, "%s%s", "/dev/", cmd->file_name);
    do {
        if (chmod(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) >= 0)
            break;
        usleep(100000);
        retry_cnt--;
    } while (retry_cnt > 0);

    if (retry_cnt <= 0)
        ALOGE("chmod fail %s[%s]", strerror(errno), path);

    //ALOGI("device node %s", path);
    obj->inFd = open(path, O_RDWR);
    if (obj->inFd < 0) {
        ALOGE("device file %s open fail %d : %s", path, obj->inFd, strerror(errno));
        delete obj;
        sleep(2);
        return -1;
    }

    memset(path, 0, 79);
    sprintf(path, "%s%s", SAVE_DIR, cmd->desc_name);
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
        if (errno != EEXIST)
            ALOGE("mkdir fail! %s", strerror(errno));
    }

    obj->updateFileName();
    objList.push_back(obj);
    ALOGI("%s save start at %s", __func__, obj->getFileName());

    return 0;
}

void free_file(MemlogObject *obj)
{
    if (obj == nullptr)
        return;

    close(obj->inFd);
    for (auto iter = objList.begin(); iter != objList.end(); iter++) {
        if (*iter == obj) {
            ALOGI("free file object %s", obj->fileName.c_str());
            //removeList(obj->descName + "$" + obj->fileName);
            objList.erase(iter);
            delete(obj);
            break;
        }
    }
}

int pause_file(memlog_file_cmd *cmd, bool pause)
{
    for (auto iter = objList.begin(); iter != objList.end(); iter++) {
        auto obj = *iter;

            if (!strcmp(obj->descName.c_str(), cmd->desc_name) &&
                !strcmp(obj->fileName.c_str(), cmd->file_name)) {
            ALOGI("file found %s %s", obj->getFileName(), pause ? "pause" : "resume");
            obj->paused = pause;
            if (!pause)
                obj->remain = 0;
            break;
        }
    }

    return 0;
}

int processCmd(memlog_file_cmd *buf)
{
    switch(buf->cmd) {
        case CMD_CREATE:
            create_file(buf);
            break;
        case CMD_PAUSE:
            pause_file(buf, true);
            break;
        case CMD_RESUME:
            pause_file(buf, false);
            break;
        case CMD_SYNC:
            break;
        default:
            ALOGE("invalid cmd: %d", buf->cmd);
            return -1;
            break;
    }

    return buf->cmd;
}

void dump_buffer(char *buffer, int length)
{
    int i = 0;

    while(length > 0) {
	ALOGI("%x %x %x %x %x %x %x %x",
		buffer[i], buffer[i+1], buffer[i+2], buffer[i+3],
		buffer[i+4], buffer[i+5], buffer[i+6], buffer[i+7]);
	i+=8;
	length-=8;
    }
}

void CmdMonitor(void) {
    int cmdFd;
    int ret = 0;
    int n;
    memlog_file_cmd *buffer = new memlog_file_cmd();

    while(1)
    {
        cmdFd = open(PATH_CMD, O_RDWR);
        if (cmdFd < 0) {
            ALOGE("%s : fail to open %s: %s", __func__, PATH_CMD, strerror(errno));
            break;
        }

        n = read(cmdFd, buffer, sizeof(memlog_file_cmd));
        if (n < 0)
            ALOGE_IF(errno != EINTR, "%s : read err", __func__);

        if (n <= 0) {
            close(cmdFd);
            break;
        }

	if (buffer->file_name[sizeof(buffer->file_name) - 1] != 0)
		buffer->file_name[sizeof(buffer->file_name) - 1] = 0;

	if (buffer->desc_name[sizeof(buffer->desc_name) - 1] != 0)
		buffer->desc_name[sizeof(buffer->desc_name) - 1] = 0;

        //dump_buffer((char *) buffer, sizeof(memlog_file_cmd));
        ALOGI("%s: cmd %u, maxsize %lu, maxnum %u, period %lu, desc %s, file %s",
                __func__, buffer->cmd, buffer->max_file_size, buffer->max_file_num,
                buffer->polling_period, buffer->desc_name, buffer->file_name);
        ret = processCmd(buffer);

        if (ret > 0) {
            for (int i = 0 ; i < ACK_RETRY_CNT ; i++) {
                n = write(cmdFd, buffer, sizeof(memlog_file_cmd));
                //ALOGI("%s: result: %d expected: %lu", __func__, n, sizeof(memlog_file_cmd));
                if (n < 0) {
                    ALOGE("ACK fail! %s", strerror(errno));
                    usleep(ACK_RETRY_WAIT);
                } else
                    break;
            }
        } else
            ALOGE("%s cmd not found!", __func__);

        memset(buffer, 0, sizeof(memlog_file_cmd));
        close(cmdFd);
    }

    delete buffer;
}

MemlogObject * getObjfromName(void *data, unsigned int datalen) {
    std::string src = std::string((const char *) data, datalen);
    std::smatch match;
    std::regex reg("[$]");
    std::regex_search(src, match, reg);
    std::string desc = match.prefix().str();
    std::string file = match.suffix().str();

    file.resize(file.find_first_of('\0',0));
    //ALOGI("@%s [%lu] %s [%lu]", desc.c_str(), desc.size(), file.c_str(), file.size());
    for (auto iter = objList.begin(); iter != objList.end(); iter++) {
        auto obj = *iter;
        if (obj->descName == desc && obj->fileName == file) {
            ALOGI("%s %s %s found!", __func__, desc.c_str(), file.c_str());
            return obj;
        }
    }
    ALOGE("%s: obj not found!!", __func__);
    return nullptr;
}

void InputCmd(char input, void *data, const char *cmd, unsigned int datalen)
{
    ALOGI("%s write %c into [%s(%u)]", __func__, input, data, datalen);
    std::string dst_path = PATH_REQ;
    int fd, ret;
    char tmp[50];
    bool flag = false;
    char *ptr = (char *)data;

    memset(tmp, 0, 50);
    datalen = datalen > 50 ? 50 : datalen;

    for (int i = 0; i < datalen && *ptr; i++) {
        if (*ptr == '$') {
            if (flag)
                break;
            flag = true;
            tmp[i] = '/';
        } else
            tmp[i] = *ptr;
        ptr++;
    }
    dst_path = dst_path + tmp + cmd;
    fd = open(dst_path.c_str(), O_RDWR);
    if (fd < 0) {
        ALOGE("failed to open %s: %m", dst_path.c_str());
    } else {
        ret = write(fd, &input, sizeof(input));
        if (ret < 0)
            ALOGE("failed to write to %s: %m", dst_path.c_str());
        close(fd);
    }
}

void SetLevel(int level, void *data, unsigned int datalen) {
    ALOGI("%s [%c]", __func__, level + '0');
    InputCmd(level + '0', data, "/level", datalen);
}

void EnableFile(bool enabled, void *data, unsigned int datalen)
{
    ALOGI("%s [%d]", __func__, enabled);
    InputCmd(enabled + '0', data, "/enable", datalen);
}

void scanCtrlList()
{
    ALOGI("%s", __func__);
    int n, m;
    dirent **namelist_desc;
    dirent **namelist_node;

    n = chdir(PATH_REQ);
    if (n < 0) {
        ALOGE("%s: error: not found %s", PATH_REQ, strerror(errno));
        return;
    }

    n = scandir(".", &namelist_desc, NULL, alphasort);
    if (n > 0) {
        while(n--) {
            if (!strcmp(namelist_desc[n]->d_name, ".") || !strcmp(namelist_desc[n]->d_name, "..")) {
                free(namelist_desc[n]);
                continue;
            }
            m = scandir(namelist_desc[n]->d_name, &namelist_node, NULL, alphasort);
            if (m < 0) {
                ALOGE("file not found in %s", namelist_desc[n]->d_name);
                free(namelist_desc[n]);
                continue;
            }

            while(m--) {
                std::string name;
                bool enabled;
                uint8_t level;
                int type = 2;
                char buf[50];
                char info[200];
                int fd, ret;
                std::string name_base;

                if (!strcmp(namelist_node[m]->d_name, ".") || !strcmp(namelist_node[m]->d_name, "..")) {
                    free(namelist_node[m]);
                    continue;
                }
                name_base = name_base + PATH_REQ + namelist_desc[n]->d_name + "/" + namelist_node[m]->d_name;

                /* get name */
                fd = open((name_base + "/name").c_str(), O_RDONLY);
                if (fd < 0) {
                    ALOGI("%s fail %s", (name_base + "/name").c_str(), strerror(errno));
                } else {
                    ret = read(fd, buf, sizeof(buf));
                    buf[strlen(buf)-1] = 0;
                    //ALOGI("%s %s", name_base.c_str(), buf);
                    name = name + namelist_desc[n]->d_name + "$" + namelist_node[m]->d_name + "$" + buf;
                    close(fd);
                    memset(buf, 0, sizeof(buf));
                }

                /* get enabled */
                fd = open((name_base + "/enable").c_str(), O_RDONLY);
                if (fd < 0) {
                    ALOGI("%s fail %s", (name_base + "/enable").c_str(), strerror(errno));
                } else {
                    ret = read(fd, buf, sizeof(buf));
                    enabled = atoi(buf) ? true : false;
                    close(fd);
                    memset(buf, 0, sizeof(buf));
                }

                fd = open((name_base + "/level").c_str(), O_RDONLY);
                if (fd < 0) {
                    ALOGI("%s fail %s", (name_base + "/level").c_str(), strerror(errno));
                } else {
                    ret = read(fd, buf, sizeof(buf));
                    level = atoi(buf);
                    close(fd);
                    memset(buf, 0, sizeof(buf));
                }

                /* get type */
                fd = open((name_base + "/info").c_str(), O_RDONLY);
                if (fd < 0) {
                    ALOGI("%s fail %s", (name_base + "/info").c_str(), strerror(errno));
                } else {
                    ret = read(fd, info, sizeof(info));
                    char typebuf[10];
                    /* skip "type=" */
                    int i = 5, j = 0;
                    while (j < 10 && info[i] && info[i] != '\n') {
                        typebuf[j++] = info[i++];
                    }

                    //ALOGI("%s info:\n%s", __func__, info);
                    for (int i = 0 ; i < sizeof(name_type) / sizeof(const char *) ; i++) {
                        if (!strcmp(name_type[i], typebuf)) {
                            type = i;
                            break;
                        }
                    }
                    close(fd);
                    memset(buf, 0, sizeof(buf));
                }
                addList(enabled, level, type, name);
                free(namelist_node[m]);
            }
            free(namelist_desc[n]);
            free(namelist_node);
        }
        free(namelist_desc);
    }
}

void saveDumpstate() {
    char buf[4096];
    int fd, n, ret;
    size_t tot = 0;
    std::string output(SAVE_DIR);
    FILE *pFile = nullptr;

    output += "dumpstate.txt";

    fd = open(PATH_DUMPSTATE, O_RDONLY);
    if (fd < 0) {
        ALOGE("%s: dumpstate open fail %s", __func__, strerror(errno));
        goto out;
    }

    pFile = fopen(output.c_str(), "w");
    if (!pFile) {
        ALOGE("%s: output file open fail %s", __func__, strerror(errno));
        goto out;
    }

    if (fd && pFile) {
        while(1) {
            n = read(fd, buf, sizeof(buf) - 1);
            if (n > 0) {
                ret = fwrite(buf, sizeof(char), n, pFile);
                tot += n;
            } else
                break;
        }
        ALOGI("%s: %zu bytes written on %s", __func__, tot, output.c_str());
    }

out:
    if (fd >= 0)
        close(fd);
    if (pFile)
        fclose(pFile);
}

int ProcessingEventCmd(char *buf) {
    char *ptr;

    ptr = strstr(buf, EVENT_CMD_PREFIX);
    if (!ptr)
        return -1;

    ptr += strlen(EVENT_CMD_PREFIX);
    switch (*ptr) {
    case 'F':
        ALOGI("%s: file", __func__);
        ThreadLoop();
        break;
    case 'C':
        ALOGI("%s: cmd", __func__);
        CmdMonitor();
        break;
    default:
        ALOGI("%s: undefined cmd [%s]", __func__, buf);
        break;
    }

    return 0;
}

void ReceiveEvent(int fd)
{
    char buf[128];
    int n;

    n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0) {
        ALOGI("%s: read fail ret(%d)", __func__, n);
        return;
    }

    buf[n] = 0;
    ProcessingEventCmd(buf);
}

void EventMonitor(void)
{
    int evt_fd;
    struct pollfd pfd;
    int ret;

    ALOGI("%s: start", __func__);
    evt_fd = open(PATH_EVENT_DEV, O_RDWR);
    if (evt_fd < 0) {
        ALOGI("%s: fail to open evt device", __func__);
	return;
    }

    pfd.fd = evt_fd;
    pfd.events = POLLIN | POLLERR;

    do {
        ret = poll(&pfd, 1, -1);
        if (ret > 0 && (pfd.revents & POLLIN))
            ReceiveEvent(evt_fd);
    } while (true);

    close(evt_fd);
}

int main(void)
{
    umask(000);
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    CPU_SET(1, &mask);
    CPU_SET(2, &mask);
    CPU_SET(3, &mask);
    int result = sched_setaffinity(0, sizeof(mask), &mask);
    if (result < 0)
        ALOGE("setaffinity fail! %d\n", result);

    MemlogAgent_Init();
    EventMonitor();

    ALOGE("terminated!");
    return 0;
}

int MemlogObject::getHeader(fileInfo *header) {
    auto fp = fopen(getFileName(), "rb");

    if (!fp) {
        if (errno != ENOENT)
            ALOGW("%s: file open fail %s(%s)", __func__, getFileName(), strerror(errno));
        return -1;
    }
    fseek(fp, 0, SEEK_SET);

    fread(header, sizeof(fileInfo), 1, fp);
    fclose(fp);
    return 0;
}

void MemlogObject::make_header(void) {
    file_header buf;
    int fd, ret;
    FILE *pFile;

    fd = open((treeName + "/info").c_str(), O_RDONLY);
    if (fd < 0) {
        ALOGE("%s info file open fail! %s", __func__, strerror(errno));
        return;
    }

    memset(&buf, 0, sizeof(buf));
    ret = read(fd, &buf.info, sizeof(buf.info));

    //ALOGI("%s\n%s", __func__, buf.header.info);
    close(fd);

    if (ret <= 0) {
        ALOGE("%s info file read fail! %s", __func__, strerror(errno));
        return;
    }

    getHeader(&buf.header);
    buf.header.size = ret;

    if (buf.header.fp)
        pFile = fopen(getFileName(), "rb+");
    else
        pFile = fopen(getFileName(), "wb");

    if (!pFile) {
        ALOGE("%s: %s fopen fail! (%s)", __func__, getFileName(), strerror(errno));
        return;
    }

    rewind(pFile);
    /* write from start when the file is empty or full */
    if (buf.header.fp < sizeof(file_header) || buf.header.fp >= maxFileSize)
        buf.header.fp = sizeof(file_header);

    ret = fwrite(&buf, sizeof(file_header), 1, pFile);
    fseek(pFile, buf.header.fp, SEEK_SET);
    fclose(pFile);

    //ALOGI("%s %s header size[0x%lx]", __func__, getFileName(), buf.header.fp);
}

FILE * MemlogObject::get_Filep() {
    int ret;
    FILE *pFile;
    fileInfo header = {0, 0};

    ret = getHeader(&header);

    /* file create if offset is 0 */
    if(ret < 0 || !header.fp)
        make_header();
    /* file is full, write on new file */
    else if (header.fp >= maxFileSize) {
        truncate(getFileName(), header.fp);
        updateFileCnt();
        make_header();
    }

    ret = getHeader(&header);
    if (ret < 0) {
        ALOGE("%s get header fail again", __func__);
        return nullptr;
    }
    /* overwrite from size */
    pFile = fopen(getFileName(), "rb+");
    if (!pFile)
        pFile = fopen(getFileName(), "wb+");

    if (pFile)
        fseek(pFile, header.fp, SEEK_SET);

    //ALOGI("%s: start from %lu",__func__, size);
    return pFile;
}

int MemlogObject::cleanPath() {
    std::string str = SAVE_DIR;
    str = str + "/" + descName + "/";
    dirent **filelist;
    int n = scandir(str.c_str(), &filelist, NULL, NULL);

    if (n > 0) {
        int d = 0;
        for (int i = 0 ; i < n ; i++) {
            if (strstr(filelist[i]->d_name, (fileName + "_").c_str()))
                if (!std::remove((str + filelist[i]->d_name).c_str()))
                    d++;
        }
        if (d)
            ALOGI("%s: removed %d old files %s", __func__, d, fileName.c_str());
    }

    for (int i = 0 ; i < n ; i++)
        free(filelist[i]);
    free(filelist);

    return 0;
}

int64_t MemlogObject::write() {
    int n, ret;
    int64_t size = 0;
    struct pollfd pfd;
    char buf[MAX_BUF * 2];

    pfd.fd = inFd;
    pfd.events = POLLIN | POLLRDNORM;
    pfd.revents = 0;

    do {
        ret = poll(&pfd, 1, 0);
        if (pfd.revents == POLLERR) {
            //free_file(obj);
            if (pFile) {
                fclose(pFile);
                pFile = nullptr;
            }
            return -POLLERR;
        } else if (pfd.revents != (POLLIN | POLLRDNORM))
            break;

        n = read(inFd, buf, sizeof(buf));
        if (n <= 0)
            break;
        //TODO: data fitting max size

        if (!flag) {
            flag = true;
            cleanPath();
        }
        if (pFile == nullptr)
            pFile = get_Filep();
        if (pFile == nullptr) {
            ALOGE("%s: filep is not available(%s)", __func__, getFileName());
            break;
        }

        ret = fwrite(buf, sizeof(char), n, pFile);
        if (ret <= 0)
            ALOGE("no data written %s", strerror(errno));
        else
            size += ret;

        //ALOGI("%s: saved %d at %s :(now %lu)", __func__, ret, getFileName(), ftell(pFile));
    } while(1);

    if (pFile) {
        auto offset = ftell(pFile);

        if (offset < 0) {
            ALOGE("error occurred in ftell() to %s: %m", getFileName());
            return size;
        }

        fclose(pFile);
        pFile = nullptr;
        FILE *tmp = fopen(getFileName(), "rb+");
        if (!tmp) {
            ALOGE("Failed to open %s: %m", getFileName());
            return size;
        }
        rewind(tmp);
        fwrite_unlocked(&offset, sizeof(offset), 1, tmp);
        fseek(tmp, offset, SEEK_SET);
        fclose(tmp);
    }

    return size;
}

std::string MemlogObject::updateFileName(void)
{
    std::string dst;

	dst += SAVE_DIR + descName + "/" + fileName + "_" + std::to_string(curFileCnt) + ".memlog";
    fileNameList[curFileCnt] = dst;

	return dst;
}

void MemlogObject::updateFileCnt(void)
{
    curFileCnt = (curFileCnt + 1) % maxFileCnt;
    updateFileName();
    ALOGI("%s write to new file : (%s)",__func__, getFileName());
}

const char *MemlogObject::getFileName(void) {
    return fileNameList[curFileCnt].c_str();
}

const char *MemlogObject::getFileName(int i) {
    return fileNameList[i].c_str();
}
