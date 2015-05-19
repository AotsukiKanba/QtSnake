#ifndef SNAKE_H
#define SNAKE_H

#include "csnakecore.h"
#include <QObject>
#include <QtCore/QTimer>

class CSnake : public QObject
{
	Q_OBJECT

Q_SIGNALS:
	void snakeMove(QPoint to, QPoint tail);

public Q_SLOTS:
	void onChangeDirect(CSnakeCore::GameDirection d);

public:
	enum SnakeState { Inactive, Living, Dead };

	CSnake();
	~CSnake();
	void initSnake(int p);
	void goSnake();

private:
	SnakeState m_state;
	QList<QPoint> m_body;
	QTimer m_timer;
	QTimer m_itemTimer;
	CSnakeCore::GameItem m_item;
	CSnakeCore::GameDirection m_direct;

private Q_SLOTS:
	void onGoTimeOut();
};

#endif // SNAKE_H
