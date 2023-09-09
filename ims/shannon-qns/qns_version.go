//
// SAMSUNG Shannon IWLAN Services
//
// Copyright (c) 2020 Samsung Electronics Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

package qns_version

import (
    "bytes"
    "fmt"
    "os/exec"
    "path/filepath"
    "runtime"
    "strings"
    "time"
    "android/soong/android"
    "android/soong/java"
)

func getTime() string {
    t := time.Now()
    return t.Format("060102.150405")
}

func getCode(path string) string {
    cmd := exec.Command("/usr/bin/git", "-C", path, "rev-list", "--count", "--first-parent", "HEAD")
    var out bytes.Buffer
    cmd.Stdout = &out
    err := cmd.Run()
    if err != nil {
        return "0"
    }
    bytes := out.Next(out.Len() - 1)
    return string(bytes)
}

func getCommit(path string) string {
    cmd := exec.Command("/usr/bin/git", "-C", path, "rev-parse", "--short", "HEAD")
    var out bytes.Buffer
    cmd.Stdout = &out
    err := cmd.Run()
    if err != nil {
        return "000000"
    }
    bytes := out.Next(out.Len() - 1)
    return string(bytes)
}

func getChange(path string) string {
    cmd := exec.Command("/bin/sh", "-c", fmt.Sprintf("cd %s;git log -1 | grep \"Change-Id:\"", path))
    var out bytes.Buffer
    cmd.Stdout = &out
    err := cmd.Run()
    if err != nil {
        return getCommit(path)
    }
    str := string(out.Next(out.Len() - 1));
    return str[strings.Index(str, ":") + 1:]
}

func getVersion(path string) string {
    return getChange(path)
}

func getVersionAndCode(path string) []string {
    var version []string

    version = append(version, "--version-name")
    version = append(version, getVersion(path))
    version = append(version, "--version-code")
    version = append(version, getCode(path))
    return version
}

func getPath(ctx android.LoadHookContext) string {
    _, filename, _, ok := runtime.Caller(0)
    if !ok {
        path, err := filepath.Abs(ctx.ModuleDir())
        if err != nil {
            return ctx.ModuleDir()
        }
        return path
    }
    return filepath.Dir(filename)
}

func myVersion(ctx android.LoadHookContext) {
    path := getPath(ctx)

    type props struct {
        Aaptflags []string
    }

    p := &props{}
    p.Aaptflags = getVersionAndCode(path)

    ctx.AppendProperties(p)
}

func init() {
    android.RegisterModuleType("qns_version", myFactory)
}

func myFactory() android.Module {
    module := java.DefaultsFactory()
    android.AddLoadHook(module, myVersion)

    return module
}
