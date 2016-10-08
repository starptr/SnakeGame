#include "GameScreen.h"
#include <QPainter>
#include <QKeyEvent>
#include <iostream>
#include <fstream>
#include <vector>

GameScreen::GameScreen(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setMinimumSize(801, 601);
	this->setMaximumSize(801, 601);
	//this->resize(800, 600);

	mode = MODE_MENU;

	init();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	//timer->start(50);

	lastTime = clock();
	respawn_last = clock();
	spam_last = clock();
}


GameScreen::~GameScreen()
{

}

void GameScreen::paintEvent(QPaintEvent * event) {


	if (r >= 255) {
		r_dir = RANGE_DOWN;
		g_dir = RANGE_UP;
		b_dir = RANGE_IDLE;
	}

	if (g >= 255) {
		r_dir = RANGE_IDLE;
		g_dir = RANGE_DOWN;
		b_dir = RANGE_UP;
	}

	if (b >= 255) {
		r_dir = RANGE_UP;
		g_dir = RANGE_IDLE;
		b_dir = RANGE_DOWN;
	}

	if (r_dir == RANGE_UP) {
		r += 5;
	}
	else if (r_dir == RANGE_DOWN){
		r -= 5;
	}
	if (g_dir == RANGE_UP) {
		g += 5;
	}
	else if (g_dir == RANGE_DOWN){
		g -= 5;
	}
	if (b_dir == RANGE_UP) {
		b += 5;
	}
	else if (b_dir == RANGE_DOWN) {
		b -= 5;
	}

	QColor white(255, 255, 255);
	QColor darkWhite(250, 250, 250);
	QColor pink(255, 192, 203);
	QColor brown(97, 25, 11);
	QColor rainbow(r, g, b);
	QColor neg_rainbow(255 - r, 255 - g, 255 - b);

	QPainter painter(this);
	painter.fillRect(0, 0, width(), height(), darkWhite);
	painter.setPen(QPen(Qt::black, 1));

	if (mode == MODE_MENU) {
		painter.drawText(400, 300, "Press the Space Key to Start");
	}
	else if (mode == MODE_RUNNING) {

		//draw coffee
		painter.setPen(QPen(darkWhite, 1));
		for (int i = 0; i < coffee_loc.size(); i++) {
			painter.drawRect(coffee_loc[i].first * sq_s, coffee_loc[i].second * sq_s, sq_s, sq_s);
			painter.fillRect(coffee_loc[i].first * sq_s + 1, coffee_loc[i].second * sq_s + 1, sq_s - 1, sq_s - 1, rainbow);
		}

		//draw fruit
		painter.setPen(QPen(pink, 1));
		painter.setBrush(pink);
		for (int i = 0; i < fruit_loc.size(); i++) {
			painter.drawEllipse((fruit_loc[i].first * sq_s), (fruit_loc[i].second * sq_s), sq_s, sq_s);
		}

		//draw snake
		painter.setPen(QPen(darkWhite, 1));
		for (int i = 0; i < body.size(); i++) {
			painter.drawRect(body[i].first * sq_s, body[i].second * sq_s, sq_s, sq_s);
			painter.fillRect(body[i].first * sq_s + 1, body[i].second * sq_s + 1, sq_s - 1, sq_s - 1, neg_rainbow);
		}

	}
	else if (mode == MODE_GAMEOVER) {
		painter.setPen(QPen(Qt::lightGray, 1));
		for (int i = 0; i < body.size(); i++) {
			painter.drawRect(body[i].first * sq_s, body[i].second * sq_s, sq_s, sq_s);
			painter.fillRect(body[i].first * sq_s + 1, body[i].second * sq_s + 1, sq_s - 1, sq_s - 1, Qt::gray);
		}
		painter.setPen(QPen(Qt::red, 1));
		painter.drawText(400, 300, "GAMEOVER");
	}







}

void GameScreen::keyPressEvent(QKeyEvent* event) {
	if (mode == MODE_RUNNING) {
		if (event->key() == Qt::Key_Space) {
			if (pause == false) {
				pause = true;
				timer->stop();
			}
			else if (pause == true) {
				pause = false;
				timer->start(50);
				lastTime = clock();
				respawn_last = clock();
				spam_last = clock();
			}
		}
		else if (event->key() == Qt::Key_Up) {
			if (prev_dir != 3 && pause == false) {
				dir = 1;
			}
		}
		else if (event->key() == Qt::Key_Down) {
			if (prev_dir != 1 && pause == false) {
				dir = 3;
			}
		}
		else if (event->key() == Qt::Key_Left) {
			if (prev_dir != 0 && pause == false) {
				dir = 2;
			}
		}
		else if (event->key() == Qt::Key_Right) {
			if (prev_dir != 2 && pause == false) {
				dir = 0;
			}
		}

	}

	else if (mode == MODE_MENU) {
		if (event->key() == Qt::Key_Space) {
			mode = MODE_RUNNING;

			init();

			timer->start(50);
		}
	}

	else if (mode == MODE_GAMEOVER) {
		if (event->key() == Qt::Key_Space) {
			mode = MODE_RUNNING;

			init();

			timer->start(50);
		}
	}

	update();
}

void GameScreen::keyReleaseEvent(QKeyEvent* event) {

}

void GameScreen::onTimer() {
	time_t now = clock();
	/*
	if ((double)(now - spam_last) > 10000) {

	spam_last = now;
	}
	*/
	if ((double)(now - respawn_last) > 5000) {
		x_rand = rand() % 80;
		y_rand = rand() % 60;
		fruit_loc.push_back(std::make_pair(x_rand, y_rand));
		x_rand = rand() % 80;
		y_rand = rand() % 60;
		coffee_loc.push_back(std::make_pair(x_rand, y_rand));
		respawn_last = now;
	}
	if ((double)(now - lastTime) > 1000.0 / (9 + coffee)) {
		// move

		int x = body.back().first;
		int y = body.back().second;

		if (dir == 0) {
			x++;
		}
		else if (dir == 1) {
			y--;
		}
		else if (dir == 2) {
			x--;
		}
		else if (dir == 3) {
			y++;
		}
		if (x > 80 - 1) {
			x = 0;
		}
		if (x < 0) {
			x = 80 - 1;
		}
		if (y > 60 - 1) {
			y = 0;
		}
		if (y < 0) {
			y = 60 - 1;
		}

		for (int i = 0; i < body.size(); i++) {
			if (body[i].first == x && body[i].second == y) {
				timer->stop();
				mode = MODE_GAMEOVER;
				break;
			}
		}

		for (int i = 0; i < fruit_loc.size(); i++) {
			if (fruit_loc[i].first == x && fruit_loc[i].second == y) {
				fruit++;
				fruit_loc.erase(fruit_loc.begin() + i);
			}
		}

		for (int i = 0; i < coffee_loc.size(); i++) {
			if (coffee_loc[i].first == x && coffee_loc[i].second == y) {
				coffee++;
				coffee_loc.erase(coffee_loc.begin() + i);
			}
		}



		body.push_back(std::make_pair(x, y));

		if (body.size() >= size_max + fruit) {
			body.erase(body.begin());
		}

		lastTime = now;
		prev_dir = dir;

		update();
	}
}

void GameScreen::init() {
	int x = 40;
	int y = 30;

	r = 255;
	g = 0;
	b = 0;

	r_dir = RANGE_DOWN;
	g_dir = RANGE_UP;
	b_dir = RANGE_IDLE;


	dir = rand() % 4;
	prev_dir = dir;

	pause = false;

	fruit = 0;
	coffee = 0;

	size_max = 4;

	sq_s = 10;

	body.clear();
	body.push_back(std::make_pair(x, y));

	fruit_loc.clear();
	coffee_loc.clear();

	for (int i = 0; i < 50; i++) {
		x_rand = rand() % 80;
		y_rand = rand() % 60;
		fruit_loc.push_back(std::make_pair(x_rand, y_rand));
		x_rand = rand() % 80;
		y_rand = rand() % 60;
		coffee_loc.push_back(std::make_pair(x_rand, y_rand));
	}

}