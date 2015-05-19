#include "csnake.h"

CSnake::CSnake()
	: m_state(Inactive)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onGoTimeOut()));
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
	m_itemTimer.stop();
	m_item = CSnakeCore::None;
	m_body.clear();

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
		m_direct = CSnakeCore::Right;
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;
	}
}

void CSnake::goSnake()
{
	m_state = Living;
	m_timer.start();
}

void CSnake::onChangeDirect(CSnakeCore::GameDirection d)
{
	m_direct = d;
}

void CSnake::onGoTimeOut()
{
	QPoint to, tail, head;
	tail = m_body.back();
	head = m_body.front();
	switch (m_direct)
	{
	case CSnakeCore::Up:
		to.setX(head.x());
		to.setY(head.y() + 1);
		break;

	case CSnakeCore::Dwon:
		to.setX(head.x());
		to.setY(head.y() - 1);
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

	m_body.pop_back();
	m_body.push_front(to);
	emit snakeMove(to, tail);
}
