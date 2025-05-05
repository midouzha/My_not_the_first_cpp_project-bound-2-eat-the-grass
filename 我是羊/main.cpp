#include <string>
#include <vector>
#include <conio.h>
#include <easyx.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;
const float pix = 40;
const int width = 800;
const int height = 640;
const clock_t FPS = 1000 / 60;
const int EAT_DISTANCE = 10;
const int GRASS_LIMIT = 10;
COLOR16 GRAY = 0x333333;

class Button {
private:
    int x;
    int y;
    int width;
    int height;
    string text;
    bool isHovered;

public:
    Button(int _x, int _y, int _width, int _height, const std::string& _text)
        : x(_x), y(_y), width(_width), height(_height), text(_text), isHovered(false) {
    }

    // 绘制按钮
    void draw() {
        ExMessage bmsg;
        peekmessage(&bmsg, EX_MOUSE);
        isHovered = isMouseHover(bmsg.x, bmsg.y);
        if (isHovered) {
            setfillcolor(LIGHTGRAY);
        }
        else {
            setfillcolor(GRAY);
        }
        fillrectangle(x, y, x + width, y + height);

        settextcolor(WHITE);
        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = 20; 
        settextstyle(&f);
        int textX = x + (width - textwidth(text.c_str())) / 2;
        int textY = y + (height - textheight(text.c_str())) / 2;
        outtextxy(textX, textY, text.c_str());
    }

    bool isMouseHover(int mouseX, int mouseY) {
        return (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
    }

    bool isClicked(int mouseX, int mouseY,ExMessage &msg) {
        return isMouseHover(mouseX, mouseY)||msg.message==WM_LBUTTONDOWN;
    }
};

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

    bool isInsideRect(int left, int top, int right, int bottom) {
        int x_c = x + (image->getwidth() / 2);
        int y_c = y + (image->getheight() / 2);
        return x_c >= left && x_c <= right && y_c >= top && y_c <= bottom;
    }
};

// 栅栏类
class Fence {
private:
    int left, top, right, bottom;
public:
    Fence(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

    void draw() {
        setlinestyle(PS_SOLID, 5);
        setlinecolor(BROWN);
        rectangle(left, top, right, bottom);
    }

    int getLeft() const { return left; }
    int getTop() const { return top; }
    int getRight() const { return right; }
    int getBottom() const { return bottom; }
};

// 游戏类
class Game {
private:
    int stage;
    float seed;
    IMAGE start, bk1, sheep_small_img, sheep_small_mask, sheep_big_img, sheep_big_mask, grassImage, grassMask;
    std::vector<Grass> grasses;
    std::vector<Sheep> sheeps;
    std::vector<Fence> rectangles;
    int grassCount;
    int round;
    int bestScore;
    bool gameOver;

public:
    Game() : stage(0), seed(300), grassCount(GRASS_LIMIT), round(0), bestScore(0), gameOver(false) {
        srand(static_cast<unsigned int>(time(nullptr)));
        initgraph(width, height);

        loadimage(&start, _T("assets/start.png"));
        loadimage(&bk1, _T("assets/bg1.png"));
        loadimage(&sheep_small_img, _T("assets/sheeps3cm.png"));
        loadimage(&sheep_small_mask, _T("assets/sheeps3cm_mask.png"));
        loadimage(&sheep_big_img, _T("assets/sheeps5.png"));
        loadimage(&sheep_big_mask, _T("assets/sheeps5_mask.png"));
        loadimage(&grassImage, _T("assets/grass1.png"));
        loadimage(&grassMask, _T("assets/grass1_mask.png"));

        sheeps.emplace_back(width / 3, height / 2, 5, &sheep_small_img, &sheep_small_mask);
        sheeps.emplace_back(width * 2 / 3, height / 4, 4, &sheep_big_img, &sheep_big_mask);
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

    void generateRectangles() {
        rectangles.clear();
        for (int i = 0; i < 2; ++i) {
            int left = rand() % (width - 100);
            int top = rand() % (height - 100);
            int right = left + 100;
            int bottom = top + 100;
            rectangles.emplace_back(left, top, right, bottom);
        }
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
                    generateRectangles();
                }
                else if (stage == 1 && grassCount > 0) {
                    grasses.emplace_back(msg.x, msg.y, &grassImage, &grassMask);
                    grassCount--;
                }
                else if (stage == 1 && gameOver) {
                    if (msg.x>= width - 120&& msg.x <= width&& msg.y>= height - 40&& msg.y <= height&&
                        msg.message==WM_LBUTTONDOWN) {
                        gameOver = false;
                        stage = 1;
                        grassCount = GRASS_LIMIT;
                        grasses.clear();
                        generateRectangles();
                        round = 0;
                    }
                }
            }
        }
    }

    bool checkWin() {
        return (sheeps[0].isInsideRect(rectangles[0].getLeft(), rectangles[0].getTop(), rectangles[0].getRight(), rectangles[0].getBottom()) &&
            sheeps[1].isInsideRect(rectangles[1].getLeft(), rectangles[1].getTop(), rectangles[1].getRight(), rectangles[1].getBottom())) ||
            (sheeps[1].isInsideRect(rectangles[0].getLeft(), rectangles[0].getTop(), rectangles[0].getRight(), rectangles[0].getBottom()) &&
                sheeps[0].isInsideRect(rectangles[1].getLeft(), rectangles[1].getTop(), rectangles[1].getRight(), rectangles[1].getBottom()));
    }

    void update() {
        if (stage == 1 && !gameOver) {
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

            if (checkWin()) {
                round++;
                if (round > bestScore) {
                    bestScore = round;
                }
                grassCount = GRASS_LIMIT;
                grasses.clear();
                generateRectangles();
            }
            else if (grassCount == 0 && grasses.empty() && !checkWin()) {
                gameOver = true;
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
            for (auto& rect : rectangles) {
                rect.draw();
            }

            settextcolor(WHITE);
            LOGFONT f;
            gettextstyle(&f);
            f.lfHeight = 25; 
            settextstyle(&f);
            char info[100];
            sprintf_s(info, "游戏模式: 栅栏挑战  草数量: %d  回合数: %d  最好成绩: %d", grassCount, round, bestScore);
            outtextxy(10, 10, info);

            if (gameOver) {
                settextcolor(RED);
                char gameOverMsg[100];
                sprintf_s(gameOverMsg, "游戏失败！最高连胜回合数: %d", bestScore);
                outtextxy(width / 2 - textwidth(gameOverMsg) / 2, height / 2, gameOverMsg);
                Button restart = Button(width - 120, height - 40, 120, 40, "重新来过！");
                restart.draw();
            }
        }
        EndBatchDraw();

        clock_t procedureTime = clock() - startTime;
        if (FPS - procedureTime > 0)
            Sleep(FPS - procedureTime);
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