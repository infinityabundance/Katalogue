#pragma once

#include <QObject>

class KatalogueDaemon : public QObject {
    Q_OBJECT
public:
    explicit KatalogueDaemon(QObject *parent = nullptr);
};
