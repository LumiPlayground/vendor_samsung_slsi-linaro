#include <hardware/exynos/hdr10pMetaInterface.h>
#define NULL (0)

class hdr10pMetaImplementation: public hdr10pMetaInterface {
public:
    hdr10pMetaImplementation() {}
    ~hdr10pMetaImplementation() {}
};

hdr10pMetaInterface *hdr10pMetaInterface::createInstance() {
    return NULL;
}
