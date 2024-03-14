#include "config.hpp"
#include "DBQuery.hpp"

#include <cinttypes>

namespace db = server::db;

int main(int argc, char **argv) {
    db::DBQuery dbquery;
    dbquery.output_all_users();

    return 0;
}