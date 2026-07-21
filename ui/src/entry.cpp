module;

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(waywallen_uiPlugin)

module waywallen.entry;

import ncrequest;
import rstd.cppstd;
import waywallen;

namespace waywallen
{
int run(int argc, char** argv) {
    auto request_init = ncrequest::global_init();
    if (request_init.is_err()) {
        auto error = rstd::cppstd::to_string(
            rstd::format("ncrequest initialization failed: {}", request_init.unwrap_err()));
        qCritical("%s", error.c_str());
        return 1;
    }

    QGuiApplication gui_app(argc, argv);
    gui_app.setDesktopFileName(APP_ID);
    gui_app.setOrganizationName("waywallen");
    gui_app.setOrganizationDomain("waywallen.org");
    gui_app.setApplicationName(APP_NAME);
    gui_app.setApplicationVersion(APP_VERSION);

    // 加载翻译
    QTranslator qtTranslator;
    QTranslator appTranslator;
    QLocale locale;
    QStringList dataDirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);

    if (qtTranslator.load(locale, "qt", "_",
            QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        gui_app.installTranslator(&qtTranslator);
    }

    QString localeName = locale.name();
    for (const auto& dir : dataDirs) {
        QString transDir = dir + "/translations";
        if (QDir(transDir).exists()) {
            QString qmFile = transDir + "/" APP_NAME "_" + localeName + ".qm";
            if (QFileInfo::exists(qmFile) && appTranslator.load(qmFile)) {
                gui_app.installTranslator(&appTranslator);
                break;
            }
        }
    }

    // 从插件目录加载翻译
    for (const auto& dir : dataDirs) {
        QString pluginsDir = dir + "/plugins";
        if (!QDir(pluginsDir).exists()) continue;
        for (const auto& plugin : QDir(pluginsDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString qmFile = pluginsDir + "/" + plugin + "/translations/" APP_NAME "_" + localeName + ".qm";
            if (QFileInfo::exists(qmFile)) {
                QTranslator* pluginTrans = new QTranslator(&gui_app);
                if (pluginTrans->load(qmFile)) {
                    gui_app.installTranslator(pluginTrans);
                }
            }
        }
    }

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(
        { "ws-port", "Override the WebSocket port (normally discovered via DBus).", "port" });
    parser.process(gui_app);

    quint16 ws_port = 0;
    if (parser.isSet("ws-port")) {
        bool ok = false;
        ws_port = parser.value("ws-port").toUShort(&ok);
        if (! ok) {
            qCritical("invalid --ws-port value: %s", qPrintable(parser.value("ws-port")));
            return 1;
        }
    }

    App app(ws_port, {});
    app.init();

    return gui_app.exec();
}
} // namespace waywallen
