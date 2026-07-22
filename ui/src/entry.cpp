module;

#include <QCommandLineParser>
#include <QDir>
#include <QGuiApplication>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(waywallen_uiPlugin)

module waywallen.entry;

import ncrequest;
import waywallen;

namespace
{
QStringList translationPaths() {
    QStringList paths;

    // A language pack is an ordinary Waywallen plugin whose compiled Qt
    // catalogues live in translations/. Search user plugins before bundled
    // catalogues so an installed language pack can update translations
    // independently of the application binary.
    for (const auto& data_root :
         QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        const QDir plugins_dir(data_root + QStringLiteral("/waywallen/plugins"));
        const auto plugin_ids =
            plugins_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const auto& plugin_id : plugin_ids) {
            paths.append(plugins_dir.filePath(plugin_id + QStringLiteral("/translations")));
        }
    }

    paths.append(
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
        QStringLiteral("/waywallen/translations"));
    paths.append(QCoreApplication::applicationDirPath() + QStringLiteral("/translations"));
    paths.append(QCoreApplication::applicationDirPath() +
                 QStringLiteral("/../share/waywallen/translations"));
    paths.removeDuplicates();
    return paths;
}
} // namespace

namespace waywallen
{
int run(int argc, char** argv) {
    ncrequest::global_init();

    QGuiApplication gui_app(argc, argv);
    gui_app.setDesktopFileName(APP_ID);
    gui_app.setOrganizationName("waywallen");
    gui_app.setOrganizationDomain("waywallen.org");
    gui_app.setApplicationName(APP_NAME);
    gui_app.setApplicationVersion(APP_VERSION);

    // Load a user- or installation-provided Qt translation. Keeping the
    // translator alive for the whole event loop is required by Qt.
    QTranslator translator;
    const QString locale_name = qEnvironmentVariable(
        "WAYWALLEN_LOCALE", QLocale::system().name());
    const QLocale requested_locale(locale_name);
    for (const auto& path : translationPaths()) {
        if (translator.load(requested_locale, QStringLiteral("waywallen"),
                            QStringLiteral("_"), path)) {
            gui_app.installTranslator(&translator);
            break;
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
