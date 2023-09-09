#include <hardware/exynos/hdrInterface.h>
#define NULL (0)

class hdrImplementation: public hdrInterface {
public:
    hdrImplementation() {}
    ~hdrImplementation() {}
};

hdrInterface *hdrInterface::createInstance() {
    return NULL;
}
