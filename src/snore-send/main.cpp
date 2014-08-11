
#include "core/snore.h"
#include "core/notification/notification.h"
#include "core/log.h"
#include "core/version.h"

#include <QApplication>
#include <QCommandLineParser>


#ifdef HAVE_KDE5
#include <KAboutData>
#include <KI18n/KLocalizedString>
#endif

#include <iostream>

using namespace Snore;


int main ( int argc, char *argv[] )
{
    QApplication app ( argc, argv );
    app.setApplicationName("snore-send");
    app.setOrganizationName("snore-send");
    app.setApplicationVersion(Snore::Version::version());

#ifdef HAVE_KDE5
    KAboutData about("snore-send", "snore-send",Snore::Version::version(),
                     i18n("A command line interface for Snorenotify."),KAboutLicense::LGPL_V3, i18n("Copyright (c) 2014 Patrick von Reth <vonreth@kde.org>"));

    KAboutData::setApplicationData(about);
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription("A command line interface for Snorenotify.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption title(QStringList() << "t" << "title", "Set the notification title.", "title");
    parser.addOption(title);

    QCommandLineOption message(QStringList() << "m" << "message", "Set the notification body.", "message");
    parser.addOption(message);

    QCommandLineOption applicationName(QStringList() << "a" << "application", "Set the notification applicattion.", "application", app.applicationName());
    parser.addOption(applicationName);


    QCommandLineOption alertName(QStringList() << "c" << "alert", "Set the notification alert class.","alert", "Default");
    parser.addOption(alertName);

    QCommandLineOption iconPath(QStringList() << "i" << "icon", "Set the notification icon.","icon", ":/root/snore.png");
    parser.addOption(iconPath);

    QCommandLineOption backend(QStringList() << "b" << "backend", "Set the notification backend.","backend");
    parser.addOption(backend);


    parser.process(app);
    snoreDebug( SNORE_DEBUG ) << parser.positionalArguments();
    if(parser.isSet(title) && parser.isSet(message))
    {
        SnoreCore core;

        core.loadPlugins(SnorePlugin::BACKEND);
        if(parser.isSet(backend)?!core.setPrimaryNotificationBackend(parser.value(backend)):!core.setPrimaryNotificationBackend())
        {
            std::cout << "Failed to set backend: " << qPrintable(parser.value(backend)) << " avalible backends: " << qPrintable(core.notificationBackends().join(", ")) << std::endl;
            return 1;
        }
        Icon icon(parser.value(iconPath));
        Application application(parser.value(applicationName),icon);
        Alert alert(parser.value(alertName),icon);
        application.addAlert(alert);

        core.registerApplication(application);

        Notification n(application, alert, parser.value(title), parser.value(message),icon);

        core.broadcastNotification(n);
        int returnCode = -1;
        app.connect(&core, &SnoreCore::notificationClosed, [&returnCode](Notification noti){
            QString reason;
            QDebug(&reason) << noti.closeReason();
            std::cout << qPrintable(reason) << std::endl;
            returnCode = noti.closeReason();
        });
        app.connect(&core, &SnoreCore::notificationClosed, &app, &QApplication::quit);
        app.exec();
        return returnCode;
    }
    parser.showHelp(1);
}

