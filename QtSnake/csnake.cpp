#include "csnake.h"

#define SNAKE CSnakeCore::GameObject(CSnakeCore::SnakeP1 + m_id)

CSnake::CSnake()
	: m_state(Inactive)
	, m_id(-1)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onGoTimeOut()));
//	connect(&m_itemTimer, SIGNAL(timeout()), this, SLOT(checkItemTimeOut()));
}

CSnake::~CSnake()
{

}

void CSnake::initSnake(int p)
{
	Q_ASSERT(p >= 1 && p <= 4);

	m_state = Inactive;
	m_timer.stop();
	m_timer.setInterval(CSnakeCore::speedBySize(3));
//	m_itemTimer.stop();
	m_item = CSnakeCore::None;
	m_body.clear();
	m_itemSurplus = 0;

	int w2 = MAP_WIDTH / 2;
	int h2 = MAP_HEIGHT / 2;
	int left = 1;
	int top = 1;
	int right = MAP_WIDTH - 2;
	int bottom = MAP_HEIGHT - 2;

	switch (p)
	{
	case 1:
		m_body.push_back(QPoint(left + 3, h2));
		m_body.push_back(QPoint(left + 2, h2));
		m_body.push_back(QPoint(left + 1, h2));
		emit setBlock(h2, left + 3, SNAKE);
		emit setBlock(h2, left + 2, SNAKE);
		emit setBlock(h2, left + 1, SNAKE);
		m_direct = CSnakeCore::Right;
		break;

	case 2:
		m_body.push_back(QPoint(right - 3, h2));
		m_body.push_back(QPoint(right - 2, h2));
		m_body.push_back(QPoint(right - 1, h2));
		emit setBlock(h2, right - 3, SNAKE);
		emit setBlock(h2, right - 2, SNAKE);
		emit setBlock(h2, right - 1, SNAKE);
		m_direct = CSnakeCore::Left;
		break;

	case 3:
		m_body.push_back(QPoint(w2, top + 3));
		m_body.push_back(QPoint(w2, top + 2));
		m_body.push_back(QPoint(w2, top + 1));
		emit setBlock(top + 3, w2, SNAKE);
		emit setBlock(top + 2, w2, SNAKE);
		emit setBlock(top + 1, w2, SNAKE);
		m_direct = CSnakeCore::Down;
		break;

	case 4:
		m_body.push_back(QPoint(w2, bottom - 3));
		m_body.push_back(QPoint(w2, bottom - 2));
		m_body.push_back(QPoint(w2, bottom - 1));
		emit setBlock(bottom - 3, w2, SNAKE);
		emit setBlock(bottom - 2, w2, SNAKE);
		emit setBlock(bottom - 1, w2, SNAKE);
		m_direct = CSnakeCore::Up;
		break;
	}
	m_nextDirect = m_direct;
}

void CSnake::goSnake()
{
	m_state = Living;
	m_timer.start();
}

void CSnake::onChangeDirect(CSnakeCore::GameDirection d)
{
	if ((m_direct + 2) % 4 != d)
		m_nextDirect = d;
}

void CSnake::onGoTimeOut()
{
	QPoint to, tail, head;
	m_direct = m_nextDirect;
	tail = m_body.back();
	head = m_body.front();
	switch (m_direct)
	{
	case CSnakeCore::Up:
		to.setX(head.x());
		to.setY(head.y() - 1);
		break;

	case CSnakeCore::Down:
		to.setX(head.x());
		to.setY(head.y() + 1);
		break;

	case CSnakeCore::Left:
		to.setX(head.x() - 1);
		to.setY(head.y());
		break;

	case CSnakeCore::Right:
		to.setX(head.x() + 1);
		to.setY(head.y());
		break;
	}

	emit snakeMove(m_id, to, tail);
}

void CSnake::die()
{
	stopSnake();
	m_state = Dead;

	for (int i = 0; m_body.begin() + i != m_body.end(); ++i)
		emit setBlock(m_body[i].y(), m_body[i].x(), CSnakeCore::Corpse);
}

void CSnake::move(QPoint to)
{
	if (m_state != Living)
		return ;

	QPoint tail = m_body.back();
	m_body.pop_back();
	m_body.push_front(to);

	if (m_itemSurplus > 0)
	{
		grow(tail);
		--m_itemSurplus;
	}
}

void CSnake::setId(int id)
{
	m_id = id;
}

void CSnake::grow(QPoint tail)
{
	m_body.push_back(tail);
	emit setBlock(tail.y(), tail.x(), SNAKE);

	int x = 0;
	if (m_item ==  CSnakeCore::SpeedUp)
		x = -CSnakeCore::speedBySize(m_body.length()) / 2;
	else if (m_item == CSnakeCore::SpeedDown)
		x = CSnakeCore::speedBySize(m_body.length());

	m_timer.stop();
	m_timer.setInterval(CSnakeCore::speedBySize(m_body.length()) + x);
	m_timer.start();
}

bool CSnake::isLiving()
{
	return m_state == Living;
}

void CSnake::stopSnake()
{
	m_timer.stop();
//	m_itemTimer.stop();
}

void CSnake::checkItemTimeOut()
{

}

void CSnake::getItem(CSnakeCore::GameItem item)
{
	m_item = item;
	m_itemSurplus = 0;
	int x = 0;

	switch (m_item)
	{
	case CSnakeCore::Longer:
		m_itemSurplus = 5;
		break;

	case CSnakeCore::SpeedUp:
		x = -CSnakeCore::speedBySize(m_body.length()) / 2;
		break;

	case CSnakeCore::SpeedDown:
		x = CSnakeCore::speedBySize(m_body.length());
		break;
	}

	if (x != 0)
	{
		m_timer.stop();
		m_timer.setInterval(CSnakeCore::speedBySize(m_body.length()) + x);
		m_timer.start();
	}
}

int CSnake::size()
{
	return m_state == Inactive ? 0 : m_body.size();
}
