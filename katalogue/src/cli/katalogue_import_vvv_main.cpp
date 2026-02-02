#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QTextStream out(stderr);
    out << "katalogue-import-vvv is not implemented in this build.\n"
           "Rebuild with Firebird client integration and ENABLE_VVV_IMPORT=ON.\n";
    return 1;
}
