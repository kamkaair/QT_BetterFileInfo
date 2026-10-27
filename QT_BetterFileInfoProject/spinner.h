#ifndef SPINNER_H
#define SPINNER_H

#include <QGraphicsScene>
#include <QMovie>
#include <QLabel>

class Spinner {
public:
    Spinner(QSize spriteSize, QLabel* target) {
        animSprite = target;

        movie = new QMovie(QCoreApplication::applicationDirPath() + "/assets/loading.gif");
        if(!movie->isValid())
            return;

        movie->setScaledSize(spriteSize);
    }

    void startMovie() {
        animSprite->setMovie(movie);
        movie->start();
    }
    void endMovie(QString inText) {
        animSprite->setText(inText);
        movie->stop();
    }
    QMovie::MovieState movieState() { return movie->state(); }
private:
    QGraphicsScene scene;
    QMovie* movie;
    QLabel* animSprite;
};

#endif // SPINNER_H
