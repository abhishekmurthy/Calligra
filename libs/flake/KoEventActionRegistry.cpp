/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoEventActionRegistry.h"

#include <QHash>
#include <KoPluginLoader.h>
#include <kglobal.h>
#include <kdebug.h>

#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include "KoEventActionFactoryBase.h"
#include "KoEventAction.h"

class KoEventActionRegistry::Singleton
{
public:
    Singleton()
            : initDone(false) {}

    KoEventActionRegistry q;
    bool initDone;
};

K_GLOBAL_STATIC(KoEventActionRegistry::Singleton, singleton)

class KoEventActionRegistry::Private
{
public:
    QHash<QString, KoEventActionFactoryBase*> presentationEventActionFactories;
    QHash<QString, KoEventActionFactoryBase*> presentationEventActions;
    QHash<QString, KoEventActionFactoryBase*> scriptEventActionFactories;
};

KoEventActionRegistry * KoEventActionRegistry::instance()
{
    KoEventActionRegistry * registry = &(singleton->q);
    if (! singleton->initDone) {
        singleton->initDone = true;
        registry->init();
    }
    return registry;
}

KoEventActionRegistry::KoEventActionRegistry()
        : d(new Private())
{
}

KoEventActionRegistry::~KoEventActionRegistry()
{
    delete d;
}

void KoEventActionRegistry::addPresentationEventAction(KoEventActionFactoryBase * factory)
{
    const QString & action = factory->action();
    if (! action.isEmpty()) {
        d->presentationEventActionFactories.insert(factory->id(), factory);
        d->presentationEventActions.insert(action, factory);
    }
}

void KoEventActionRegistry::addScriptEventAction(KoEventActionFactoryBase * factory)
{
    d->scriptEventActionFactories.insert(factory->id(), factory);
}

QList<KoEventActionFactoryBase *> KoEventActionRegistry::presentationEventActions()
{
    return d->presentationEventActionFactories.values();
}

QList<KoEventActionFactoryBase *> KoEventActionRegistry::scriptEventActions()
{
    return d->scriptEventActionFactories.values();
}

void KoEventActionRegistry::init()
{
    KoPluginLoader::PluginsConfig config;
    config.whiteList = "PresentationEventActionPlugins";
    config.blacklist = "PresentationEventActionPluginsDisabled";
    config.group = "calligra";
    KoPluginLoader::instance()->load(QString::fromLatin1("Calligra/PresentationEventAction"),
                                     QString::fromLatin1("[X-PresentationEventAction-PluginVersion] == 27"),
                                     config);

    config.whiteList = "ScriptEventActionPlugins";
    config.blacklist = "ScriptEventActionPluginsDisabled";
    KoPluginLoader::instance()->load(QString::fromLatin1("Calligra/ScriptEventAction"),
                                     QString::fromLatin1("[X-ScriptEventAction-PluginVersion] == 27"),
                                     config);
}

QSet<KoEventAction*> KoEventActionRegistry::createEventActionsFromOdf(const KoXmlElement & e, KoShapeLoadingContext & context) const
{
    QSet<KoEventAction *> eventActions;

    if (e.namespaceURI() == KoXmlNS::office && e.tagName() == "event-listeners") {
        KoXmlElement element;
        forEachElement(element, e) {
            if (element.tagName() == "event-listener") {
                if (element.namespaceURI() == KoXmlNS::presentation) {
                    QString action(element.attributeNS(KoXmlNS::presentation, "action", QString()));
                    QHash<QString, KoEventActionFactoryBase *>::const_iterator it(d->presentationEventActions.find(action));

                    if (it != d->presentationEventActions.constEnd()) {
                        KoEventAction * eventAction = it.value()->createEventAction();
                        if (eventAction) {
                            if (eventAction->loadOdf(element, context)) {
                                eventActions.insert(eventAction);
                            } else {
                                delete eventAction;
                            }
                        }
                    } else {
                        kWarning(30006) << "presentation:event-listerer action = " << action << "not supported";
                    }
                } else if (element.namespaceURI() == KoXmlNS::script) {
                    // TODO
                } else {
                    kWarning(30006) << "element" << e.namespaceURI() << e.tagName() << "not supported";
                }
            } else {
                kWarning(30006) << "element" << e.namespaceURI() << e.tagName() << "not supported";
            }
        }
    } else {
        kWarning(30006) << "office:event-listeners not found got:" << e.namespaceURI() << e.tagName();
    }

    return eventActions;
}

