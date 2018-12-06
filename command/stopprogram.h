#ifndef STOPPROGRAM_H
#define STOPPROGRAM_H
#include "icommand.h"

#include <QObject>

class StopProgram : public QObject, public ICommand
{
    Q_OBJECT
public:
    explicit StopProgram(QObject *parent = nullptr);

    void execute() override;


signals:

public slots:
};

#endif // STOPPROGRAM_H
