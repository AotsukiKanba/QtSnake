#ifndef SNAKE_H
#define SNAKE_H

#include "csnakecore.h"
#include <QObject>
#include <QtCore/QTimer>

class CSnake : public QObject
{
	Q_OBJECT

Q_SIGNALS:
	void snakeMove(int id, QPoint to, QPoint tail);
	void setBlock(int x, int y, CSnakeCore::GameObject g);

public Q_SLOTS:
	void onChangeDirect(CSnakeCore::GameDirection d);

public:
	enum SnakeState { Inactive, Living, Dead };

	CSnake();
	~CSnake();
	void setId(int id);
	void initSnake(int p);
	void goSnake();
	void move(QPoint to);
	void die();
	void grow(QPoint tail);
	bool isLiving();
	void stopSnake();
	void getItem(CSnakeCore::GameItem item);

protected:
	void checkItemTimeOut();

private:
	int m_id;
	SnakeState m_state;
	QList<QPoint> m_body;
	QTimer m_timer;
//	QTimer m_itemTimer;
	CSnakeCore::GameItem m_item;
	CSnakeCore::GameDirection m_direct;
	CSnakeCore::GameDirection m_nextDirect;
	int m_itemSurplus;

private Q_SLOTS:
	void onGoTimeOut();
};

#endif // SNAKE_H
