#include <string>
#include <vector>
#include <conio.h>
#include <easyx.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

const float pix = 40;
const int width = 800;
const int height = 640;
const clock_t FPS = 1000 / 60;
const int EAT_DISTANCE = 10; // 设置羊距离草若干像素就可以吃草

// 草类
class Grass {
private:
    int x, y;
    bool isEaten;
    IMAGE* image;
    IMAGE* mask;
    int sheepEatingCount;
public:
    Grass(int _x, int _y, IMAGE* img, IMAGE* msk) : x(_x), y(_y), isEaten(false), image(img), mask(msk), sheepEatingCount(0) {}

    void draw() {
        if (!isEaten) {
            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, mask, NOTSRCERASE);
            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, image, SRCINVERT);
        }
    }

    bool getIsEaten() const {
        return isEaten;
    }

    void setIsEaten(bool eaten) {
        isEaten = eaten;
    }

    int getSheepEatingCount() const {
        return sheepEatingCount;
    }

    void incrementSheepEatingCount() {
        sheepEatingCount++;
    }

    void resetSheepEatingCount() {
        sheepEatingCount = 0;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }
};

// 羊类
class Sheep {
private:
    int x, y;
    int targetX, targetY;
    bool isMoving;
    int MOVE_SPEED;
    IMAGE* image;
    IMAGE* mask;
    bool isEating;
    int eatTimeLeft;
    int currentTargetGrassIndex;
public:
    Sheep(int _x, int _y, int speed, IMAGE* img, IMAGE* msk) : x(_x), y(_y), targetX(_x), targetY(_y), isMoving(false), MOVE_SPEED(speed), image(img), mask(msk), isEating(false), eatTimeLeft(100), currentTargetGrassIndex(-1) {}

    void draw() {
        putimage(x, y, mask, NOTSRCERASE);
        putimage(x, y, image, SRCINVERT);
    }

    void findNearestGrass(const std::vector<Grass>& grasses) {
        int minDistance = INT_MAX;
        int nearestGrassIndex = -1;
        for (size_t i = 0; i < grasses.size(); ++i) {
            if (!grasses[i].getIsEaten()) {
                int dx = grasses[i].getX() - x;
                int dy = grasses[i].getY() - y;
                int distance = dx * dx + dy * dy;
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestGrassIndex = static_cast<int>(i);
                }
            }
        }
        if (nearestGrassIndex != -1) {
            targetX = grasses[nearestGrassIndex].getX();
            targetY = grasses[nearestGrassIndex].getY();
            isMoving = true;
            currentTargetGrassIndex = nearestGrassIndex;
        }
    }

    void move(std::vector<Grass>& grasses) {
        if (isMoving) {
            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size() && grasses[currentTargetGrassIndex].getIsEaten()) {
                // 目标草已被吃掉，停止移动并寻找新的草
                isMoving = false;
                currentTargetGrassIndex = -1;
                findNearestGrass(grasses);
                return;
            }
            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size()) {
                int dx = targetX - x;
                int dy = targetY - y;
                float distance = static_cast<float>(sqrt(static_cast<double>(dx * dx + dy * dy)));

                if (distance > MOVE_SPEED && distance > EAT_DISTANCE) {
                    float ratio = MOVE_SPEED / distance;
                    x += static_cast<int>(dx * ratio);
                    y += static_cast<int>(dy * ratio);
                }
                else {
                    isMoving = false;
                    isEating = true;
                    if (currentTargetGrassIndex != -1) {
                        grasses[currentTargetGrassIndex].incrementSheepEatingCount();
                    }
                }
            }
        }
    }

    void eat(std::vector<Grass>& grasses) {
        if (isEating) {
            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size()) {
                Grass& currentGrass = grasses[currentTargetGrassIndex];
                for (int i = 0; i < currentGrass.getSheepEatingCount(); ++i) {
                    if (eatTimeLeft > 0) {
                        eatTimeLeft--;
                    }
                }
                if (eatTimeLeft <= 0) {
                    currentGrass.setIsEaten(true);
                    isEating = false;
                    currentTargetGrassIndex = -1;
                    currentGrass.resetSheepEatingCount();
                    findNearestGrass(grasses);
                }
            }
        }
    }
};

// 游戏类
class Game {
private:
    int stage;
    float seed;
    IMAGE start, bk1, sheep_small_img, sheep_small_mask, sheep_big_img, sheep_big_mask, grassImage, grassMask;
    std::vector<Grass> grasses;
    std::vector<Sheep> sheeps;

public:
    Game() : stage(0), seed(300) {
        srand(static_cast<unsigned int>(time(nullptr)));
        initgraph(width, height);

        // 使用相对路径加载图片
        loadimage(&start, _T("assets/start.png"));
        loadimage(&bk1, _T("assets/bg1.png"));
        loadimage(&sheep_small_img, _T("assets/sheeps3cm.png"));
        loadimage(&sheep_small_mask, _T("assets/sheeps3cm_mask.png"));
        loadimage(&sheep_big_img, _T("assets/sheeps5.png"));
        loadimage(&sheep_big_mask, _T("assets/sheeps5_mask.png"));
        loadimage(&grassImage, _T("assets/grass1.png"));
        loadimage(&grassMask, _T("assets/grass1_mask.png"));

        // 移除两种羊速度的差异，统一速度为 2
        sheeps.emplace_back(width / 3, height / 2, 5, &sheep_small_img, &sheep_small_mask);
        sheeps.emplace_back(width * 2 / 3, height / 4, 2, &sheep_big_img, &sheep_big_mask);
    }

    void showStart() {
        setbkmode(TRANSPARENT);
        settextcolor(RGB(10, 50, 10));
        char str[] = "按鼠标左键开始！";

        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = height / 15;
        f.lfWeight = static_cast<LONG>(seed);
        _tcscpy_s(f.lfFaceName, _T("萝莉体"));
        f.lfQuality = ANTIALIASED_QUALITY;
        settextstyle(&f);

        outtextxy(width / 2 - textwidth(str) / 2, height * 0.8, str);
    }

    void inoutEffect(const IMAGE& pic, int speed, bool flag)
    {
        if (flag == 0) {
            for (int i = 0; speed * i <= 400; i++) {
                clock_t startTime = clock();
                clock_t procedureTime = 0;
                cleardevice();
                BeginBatchDraw();
                putimage(speed * i, speed * 0.8 * i, 800 - 2 * speed * i, 640 - speed * 1.6 * i, &pic, speed * i, speed * 0.8 * i);
                EndBatchDraw();
                procedureTime = clock() - startTime;
                if (FPS - procedureTime > 0)
                    Sleep(FPS - procedureTime);
            }
        }
        else {
            for (int i = 400 / speed; speed * i >= 0; i--) {
                clock_t startTime = clock();
                clock_t procedureTime = 0;
                cleardevice();
                BeginBatchDraw();
                putimage(i * speed, 0.8 * i * speed, (400 - i * speed) * 2, (320 - 0.8 * i * speed) * 2, &pic, i * speed, 0.8 * i * speed);
                EndBatchDraw();
                procedureTime = clock() - startTime;
                if (FPS - procedureTime > 0)
                    Sleep(FPS - procedureTime);
            }
        }
    }

    void startEffect(int speed) {
        inoutEffect(start, speed, false);
        inoutEffect(bk1, speed, true);
    }

    void handleInput() {
        ExMessage msg;
        while (peekmessage(&msg, EX_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN) {
                if (stage == 0) {
                    setbkcolor(RGB(95, 117, 64));
                    startEffect(10);
                    stage++;
                    setbkmode(TRANSPARENT);
                }
                else if (stage == 1) {
                    grasses.emplace_back(msg.x, msg.y, &grassImage, &grassMask);
                }
            }
        }
    }

    void update() {
        if (stage == 1) {
            if (!grasses.empty()) {
                for (auto& sheep : sheeps) {
                    sheep.findNearestGrass(grasses);
                }
            }
            for (auto& sheep : sheeps) {
                sheep.move(grasses);
            }
            for (auto& sheep : sheeps) {
                sheep.eat(grasses);
            }

            auto newEnd = std::remove_if(grasses.begin(), grasses.end(), [](const Grass& grass) {
                return grass.getIsEaten();
                });
            grasses.erase(newEnd, grasses.end());

            if (!grasses.empty()) {
                for (auto& sheep : sheeps) {
                    sheep.findNearestGrass(grasses);
                }
            }
        }
    }

    void draw() {
        BeginBatchDraw();
        clock_t startTime = clock();
        if (stage == 0) {
            putimage(0, 0, &start);
            showStart();
        }
        else if (stage == 1) {
            cleardevice();
            putimage(0, 0, &bk1);
            for (auto& grass : grasses) {
                grass.draw();
            }
            for (auto& sheep : sheeps) {
                sheep.draw();
            }
        }
        EndBatchDraw();

        clock_t procedureTime = clock() - startTime;
        if (FPS - procedureTime > 0)
            Sleep(FPS - (clock() - startTime));
    }

    void run() {
        int flag = 0;
        while (true) {
            if (seed >= 900) flag = 1;
            if (seed <= 300) flag = 0;
            flag ? seed -= pix : seed += pix;

            handleInput();
            update();
            draw();
        }
    }
};

int main() {
    Game game;
    game.run();
    system("pause");
    return 0;
}