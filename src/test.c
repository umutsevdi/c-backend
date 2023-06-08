#include "router.h"
#include "token_tree.h"
#include "util.h"
#include <microhttpd.h>
#include <stdio.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>" \
             "</head><body>libmicrohttpd demo!!</body></html>"

typedef struct {
    int port;
    char* name;
} Config;

int main(int argc, char* argv[])
{
    hc_tree_test();
    hc_route_setup();
    hc_route_test();
    printf("End of tests\n");
    return 0;
}
