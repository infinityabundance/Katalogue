#include <QCoreApplication>
#include <QTextStream>

#include "katalogue_version.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    const QStringList args = app.arguments();
    if (args.contains(QStringLiteral("--version"))) {
        QTextStream out(stdout);
        out << "Katalogue VVV import tool " << KATALOGUE_VERSION_STRING << Qt::endl;
        return 0;
    }
    QTextStream out(stderr);
    out << "katalogue-import-vvv is not implemented in this build.\n"
           "Rebuild with Firebird client integration and ENABLE_VVV_IMPORT=ON.\n";
    return 1;
}
