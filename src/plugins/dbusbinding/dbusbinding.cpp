#include "dbusbinding.h"
#include <QtCore>

Q_EXPORT_PLUGIN2(dbusbinding,DBusPlugin)

void DBusPlugin::setSnore(SnoreServer *snore){
    new DBusBinding(this,snore);
}

DBusBinding::DBusBinding(DBusPlugin* parent,SnoreServer* snore):QDBusAbstractAdaptor(parent),snore(snore){
    registerTypes();    
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService( "org.SnoreNotify" );
    dbus.registerObject( "/SnoreNotify", this );
    connect(snore,SIGNAL(applicationListChanged()),this,SLOT(applicationListChangedSlot()));
}
DBusBinding::~DBusBinding(){
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService( "/SnoreNotify" );
}


void DBusBinding::registerTypes(){
    qDBusRegisterMetaType<ApplicationsList>();
    qDBusRegisterMetaType<Application>();
    qDBusRegisterMetaType<Alert>();
    qDBusRegisterMetaType<AlertList>();
}


ApplicationsList DBusBinding::getApplicationList(){
    //workaround for the "" application bug
    if(snore->getAplicationList()->contains("")){
        snore->getAplicationList()->take("").clear();
        qDebug()<<"Removed wrong entry ("") in application list";
    }

    return *snore->getAplicationList();
}

void DBusBinding::setAlertActive(const QString &application,const QString &name,const bool active){
    QSharedPointer<Application> ap(snore->getAplicationList()->value(application));
    ap->alerts.value(name)->active=active;
    emit applicationListChanged(*snore->getAplicationList());
}


void DBusBinding::applicationListChangedSlot(){
    emit applicationListChanged(*snore->getAplicationList());
}


QDBusArgument &operator<<(QDBusArgument &a, const ApplicationsList  &ap) {
    a.beginArray(qMetaTypeId<Application>());
    qDebug()<<"ApplicationList:"<<ap.keys();
    foreach(QSharedPointer<Application> appl,ap.values()){
        a<<*appl.data();
    }
    a.endArray();
    return a;
}

const QDBusArgument & operator >>(const QDBusArgument &a,  ApplicationsList  &ap) {
    a.beginArray();
    ap.clear();
    Application aplication;
    Application * ap_ptr;
    while ( !a.atEnd() ) {
        a>>aplication;
        ap_ptr=new Application(aplication.name);
        ap_ptr->alerts=aplication.alerts;
        ap.insert(aplication.name,QSharedPointer<Application>(ap_ptr));
    }
    a.endArray();
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const Application &ap){
    a.beginStructure();
    a<<ap.name<<ap.alerts;
    a.endStructure();
    return a;
}
const QDBusArgument & operator >>(const QDBusArgument &a, Application  &ap) {
    a.beginStructure();
    a>>ap.name;
    a>>ap.alerts;
    a.endStructure();
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const Alert &al){
    a.beginStructure();
    a<<al.name<<al.title<<al.active;
    a.endStructure();
    return a;
}
const QDBusArgument & operator >>(const QDBusArgument &a, Alert &al) {
    a.beginStructure();
    a>>al.name;
    a>>al.title;
    a>>al.active;    
    a.endStructure();
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const AlertList &al){
    a.beginArray(qMetaTypeId<Alert>());
    foreach(QSharedPointer<Alert> alls,al.values()){
        a<<*alls.data();
    }
    a.endArray();
    return a;
}
const QDBusArgument & operator >>(const QDBusArgument &a, AlertList &al) {
    a.beginArray();
    al.clear();
    Alert alert;
    while ( !a.atEnd() ) {
        a>>alert;
        al.insert(alert.name,QSharedPointer<Alert>(new Alert(alert.name,alert.title,alert.active)));
    }
    a.endArray();
    return a;
}

#include "dbusbinding.moc"