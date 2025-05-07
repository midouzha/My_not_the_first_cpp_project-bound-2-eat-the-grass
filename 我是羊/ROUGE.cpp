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
COLOR16 GRAY = 0x33333;
const int CHEW_TIME = 30; // 小羊咀嚼时间（帧数）
const int EDGE_MARGIN = 50; // 地图边缘距离，避免草生成在边缘
const int EAT_EXTRA_DISTANCE = 20; 

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

    void draw() {
        ExMessage bmsg;
        if (peekmessage(&bmsg, EX_MOUSE)) {
            isHovered = isMouseHover(bmsg.x, bmsg.y);
        }
        if (isHovered) {
            setfillcolor(LIGHTGRAY);
        }
        else {
            setfillcolor(GRAY);
        }
        setlinecolor(BLACK);
        fillrectangle(x, y, x + width, y + height);

        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        LOGFONT f;
        gettextstyle(&f);
        f.lfHeight = 20; // 调整按钮文字字号
        settextstyle(&f);
        int textX = x + (width - textwidth(text.c_str())) / 2;
        int textY = y + (height - textheight(text.c_str())) / 2;
        outtextxy(textX, textY, text.c_str());
    }

    bool isMouseHover(int mouseX, int mouseY) {
        return (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
    }

    bool isClicked(int mouseX, int mouseY) {
        return isMouseHover(mouseX, mouseY);
    }
};

class Grass {
private:
    int x, y;
    bool isEaten;

    IMAGE* mask;
public:
    IMAGE* image;
    Grass(int _x, int _y, IMAGE* img, IMAGE* msk) : x(_x), y(_y), isEaten(false), image(img), mask(msk) {}

    void draw() {
        if (!isEaten) {
            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, mask, NOTSRCERASE);
            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, image, SRCINVERT);
            
            setlinecolor(0x006400);
            rectangle(x - image->getwidth() / 2, y - image->getheight() / 2,
                x + image->getwidth() / 2, y + image->getheight() / 2);
        }
    }

    bool getIsEaten() const {
        return isEaten;
    }

    void setIsEaten(bool eaten) {
        isEaten = eaten;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }
};

class Sheep {
private:
    int x, y;
    int targetX, targetY;
    bool isMoving;
    int MOVE_SPEED;
    IMAGE* image;
    IMAGE* mask;
    int score;

public:
    Sheep(int _x, int _y, int speed, IMAGE* img, IMAGE* msk) : x(_x), y(_y), targetX(_x), targetY(_y), isMoving(false), MOVE_SPEED(speed), image(img), mask(msk), score(0) {}

    void draw() {
        putimage(x, y, mask, NOTSRCERASE);
        putimage(x, y, image, SRCINVERT);
    }

    void setTarget(int _targetX, int _targetY) {
        targetX = _targetX;
        targetY = _targetY;
        isMoving = true;
    }
    void setScore(int score) {
        this->score = score;
    }

    void move() {
        if (isMoving) {
            int dx = targetX - x;
            int dy = targetY - y;
            float distance = static_cast<float>(sqrt(static_cast<double>(dx * dx + dy * dy)));

            if (distance > MOVE_SPEED) {
                float ratio = MOVE_SPEED / distance;
                x += static_cast<int>(dx * ratio);
                y += static_cast<int>(dy * ratio);
            }
            else {
                isMoving = false;
            }
        }
    }

    bool isTouchingGrass(const Grass& grass) {
        int sheepCenterX = x + image->getwidth() / 2;
        int sheepCenterY = y + image->getheight() / 2;
        int grassLeft = grass.getX() - grass.image->getwidth() / 2 - EAT_EXTRA_DISTANCE;
        int grassRight = grass.getX() + grass.image->getwidth() / 2 + EAT_EXTRA_DISTANCE;
        int grassTop = grass.getY() - grass.image->getheight() / 2 - EAT_EXTRA_DISTANCE;
        int grassBottom = grass.getY() + grass.image->getheight() / 2 + EAT_EXTRA_DISTANCE;
        return (sheepCenterX >= grassLeft && sheepCenterX <= grassRight &&
            sheepCenterY >= grassTop && sheepCenterY <= grassBottom);
    }

    void eatGrass(Grass& grass) {
        if (isTouchingGrass(grass) && !grass.getIsEaten()) {
            grass.setIsEaten(true);
            score++;
        }
    }

    int getScore() const {
        return score;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void setMoveSpeed(int speed) {
        MOVE_SPEED = speed;
    }

    int getMoveSpeed() const {
        return MOVE_SPEED;
    }
};

class FollowSheep {
private:
    int x, y;
    int targetX, targetY;
    bool isMoving;
    int MOVE_SPEED;
    IMAGE* image;
    IMAGE* mask;
    Sheep* owner;
    int chewCount; 

public:
    FollowSheep(int _x, int _y, int speed, IMAGE* img, IMAGE* msk, Sheep* _owner) : x(_x), y(_y), targetX(_x), targetY(_y), isMoving(false), MOVE_SPEED(speed), image(img), mask(msk), owner(_owner), chewCount(0) {}
    void setMoveSpeed(int s) {
        this->MOVE_SPEED = s;
    }
    void draw() {
        putimage(x, y, mask, NOTSRCERASE);
        putimage(x, y, image, SRCINVERT);
    }

    void setTarget(int _targetX, int _targetY) {
        targetX = _targetX;
        targetY = _targetY;
        isMoving = true;
    }

    void move() {
        if (chewCount > 0) {
            chewCount--;
            return;
        }
        if (isMoving) {
            int dx = targetX - x;
            int dy = targetY - y;
            float distance = static_cast<float>(sqrt(static_cast<double>(dx * dx + dy * dy)));

            if (distance > MOVE_SPEED) {
                float ratio = MOVE_SPEED / distance;
                x += static_cast<int>(dx * ratio);
                y += static_cast<int>(dy * ratio);
            }
            else {
                isMoving = false;
            }
        }
    }

    void findNearestGrass(std::vector<Grass>& grasses) {
        if (chewCount > 0) return;
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
        if (nearestGrassIndex != -1 && minDistance < EAT_DISTANCE * EAT_DISTANCE) {
            grasses[nearestGrassIndex].setIsEaten(true);
            owner->setScore(owner->getScore() + 1);
            chewCount = CHEW_TIME;
        }
        else if (nearestGrassIndex != -1) {
            setTarget(grasses[nearestGrassIndex].getX(), grasses[nearestGrassIndex].getY());
        }
    }
};

class SurroundSheep {
private:
    int x, y;
    int centerX, centerY;
    float angle;
    int radius;
    int MOVE_SPEED;
    IMAGE* image;
    IMAGE* mask;
    Sheep* owner;

public:
    SurroundSheep(int _centerX, int _centerY, int _radius, int speed, IMAGE* img, IMAGE* msk, Sheep* _owner) : centerX(_centerX), centerY(_centerY), angle(0), radius(_radius), MOVE_SPEED(speed), image(img), mask(msk), owner(_owner) {
        x = centerX + radius * cos(angle);
        y = centerY + radius * sin(angle);
    }

    void setMoveSpeed(int s) {
        this->MOVE_SPEED = s;
    }

    void draw() {
        putimage(x, y, mask, NOTSRCERASE);
        putimage(x, y, image, SRCINVERT);
    }

    void move() {
        centerX = owner->getX();
        centerY = owner->getY();
        angle += 0.01 * MOVE_SPEED;
        x = centerX + radius * cos(angle);
        y = centerY + radius * sin(angle);
    }

    void findNearestGrass(std::vector<Grass>& grasses) {
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
        if (nearestGrassIndex != -1 && minDistance < EAT_DISTANCE * EAT_DISTANCE) {
            int sheepCenterX = x + image->getwidth() / 2;
            int sheepCenterY = y + image->getheight() / 2;
            int grassLeft = grasses[nearestGrassIndex].getX() - grasses[nearestGrassIndex].image->getwidth() / 2 - EAT_EXTRA_DISTANCE;
            int grassRight = grasses[nearestGrassIndex].getX() + grasses[nearestGrassIndex].image->getwidth() / 2 + EAT_EXTRA_DISTANCE;
            int grassTop = grasses[nearestGrassIndex].getY() - grasses[nearestGrassIndex].image->getheight() / 2 - EAT_EXTRA_DISTANCE;
            int grassBottom = grasses[nearestGrassIndex].getY() + grasses[nearestGrassIndex].image->getheight() / 2 + EAT_EXTRA_DISTANCE;
            if (sheepCenterX >= grassLeft && sheepCenterX <= grassRight &&
                sheepCenterY >= grassTop && sheepCenterY <= grassBottom) {
                grasses[nearestGrassIndex].setIsEaten(true);
                owner->setScore(owner->getScore() + 1);
            }
        }
    }
};

class Game {
private:
    int stage;
    float seed;
    IMAGE start, bk1, sheep_small_img, sheep_small_mask, sheep_big_img, sheep_big_mask, grassImage, grassMask;
    std::vector<Grass> grasses;
    std::vector<Sheep> sheeps;
    int grassCount;
    int bigSheepExp;
    int bigSheepLevel;
    int expToLevelUp;
    std::vector<FollowSheep> followSheeps;
    std::vector<SurroundSheep> surroundSheeps;
    int followSheepSpeed;
    int surroundSheepSpeed;
    int grassSeedCount;
    int grassGenerateRate;
    std::vector<int> lastBigSheepPositions;
    std::vector<int> currentRewards; // 用于存储当前升级的奖励选项
    int totalScore; // 总分数，独立于升级
    float grassConnoisseurFactor; // 青草鉴赏家因子
    int immortalModeBonus; // 仙人模式额外奖励选项数

public:
    Game() : stage(0), seed(300), grassCount(GRASS_LIMIT), bigSheepExp(0), bigSheepLevel(1), expToLevelUp(10),
        followSheepSpeed(3), surroundSheepSpeed(1), grassSeedCount(0), grassGenerateRate(100),
        totalScore(0), grassConnoisseurFactor(1.0), immortalModeBonus(0) {
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

        sheeps.emplace_back(width / 3, height / 2, 5, &sheep_big_img, &sheep_big_mask);
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
                    for (auto& sheep : sheeps) {
                        sheep.setTarget(msg.x, msg.y);
                    }
                    for (auto& followSheep : followSheeps) {
                        followSheep.setTarget(msg.x, msg.y);
                    }

                    if (bigSheepExp >= expToLevelUp) {
                        if (currentRewards.empty()) {
                            int numRewards = 3 + immortalModeBonus;
                            while (currentRewards.size() < numRewards) {
                                int reward = rand() % 8;
                                bool duplicate = false;
                                for (int r : currentRewards) {
                                    if (r == reward) {
                                        duplicate = true;
                                        break;
                                    }
                                }
                                if (!duplicate) {
                                    currentRewards.push_back(reward);
                                }
                            }
                        }

                        int buttonX = width / 2 - 200;
                        int buttonY = height / 2 - 50;
                        for (int i = 0; i < currentRewards.size(); ++i) {
                            std::string rewardText;
                            switch (currentRewards[i]) {
                            case 0:
                                rewardText = "信号素喷雾";
                                break;
                            case 1:
                                rewardText = "领养协议";
                                break;
                            case 2:
                                rewardText = "gogogo~";
                                break;
                            case 3:
                                rewardText = "勇敢的木";
                                break;
                            case 4:
                                rewardText = "一袋草籽";
                                break;
                            case 5:
                                rewardText = "粪便收集";
                                break;
                            case 6:
                                rewardText = "青草鉴赏家";
                                break;
                            case 7:
                                rewardText = "仙人模式";
                                break;
                            }
                            Button rewardButton(buttonX + i * 150, buttonY, 120, 40, rewardText);
                            if (rewardButton.isClicked(msg.x, msg.y)) {
                                switch (currentRewards[i]) {
                                case 0:
                                    surroundSheeps.emplace_back(sheeps[0].getX(), sheeps[0].getY(), 50, surroundSheepSpeed, &sheep_small_img, &sheep_small_mask, &sheeps[0]);
                                    break;
                                case 1:
                                    followSheeps.emplace_back(sheeps[0].getX(), sheeps[0].getY(), followSheepSpeed, &sheep_small_img, &sheep_small_mask, &sheeps[0]);
                                    break;
                                case 2:
                                    sheeps[0].setMoveSpeed(sheeps[0].getMoveSpeed() + 1);
                                    break;
                                case 3:
                                    followSheepSpeed++;
                                    surroundSheepSpeed++;
                                    for (auto& followSheep : followSheeps) {
                                        followSheep.setMoveSpeed(followSheepSpeed);
                                    }
                                    for (auto& surroundSheep : surroundSheeps) {
                                        surroundSheep.setMoveSpeed(surroundSheepSpeed);
                                    }
                                    break;
                                case 4:
                                    grassSeedCount++;
                                    break;
                                case 5:
                                    grassGenerateRate = max(10, grassGenerateRate - 10);
                                    break;
                                case 6:
                                    grassConnoisseurFactor *= 0.9;
                                    break;
                                case 7:
                                    immortalModeBonus++;
                                    break;
                                }
                                bigSheepExp = 0;
                                expToLevelUp = static_cast<int>(expToLevelUp * 2 * grassConnoisseurFactor);
                                bigSheepLevel++;
                                currentRewards.clear();
                            }
                        }
                    }
                }
            }
        }
    }

    void update() {
        if (stage == 1) {
            if (grasses.empty()) { 
                int x = rand() % (width - 2 * EDGE_MARGIN) + EDGE_MARGIN;
                int y = rand() % (height - 2 * EDGE_MARGIN) + EDGE_MARGIN;
                grasses.emplace_back(x, y, &grassImage, &grassMask);
            }
            else if (rand() % grassGenerateRate == 0) {
                int x = rand() % (width - 2 * EDGE_MARGIN) + EDGE_MARGIN;
                int y = rand() % (height - 2 * EDGE_MARGIN) + EDGE_MARGIN;
                grasses.emplace_back(x, y, &grassImage, &grassMask);
            }
            if (grassSeedCount > 0 && rand() % (18 - grassSeedCount) == 0) { //调整草籽强度
                lastBigSheepPositions.push_back(sheeps[0].getX());
                lastBigSheepPositions.push_back(sheeps[0].getY());
                if (lastBigSheepPositions.size() > 100) {
                    int x = lastBigSheepPositions[0];
                    int y = lastBigSheepPositions[1];
                    grasses.emplace_back(x, y, &grassImage, &grassMask);
                    lastBigSheepPositions.erase(lastBigSheepPositions.begin());
                    lastBigSheepPositions.erase(lastBigSheepPositions.begin());
                }
            }

            for (auto& sheep : sheeps) {
                sheep.move();
                for (auto& grass : grasses) {
                    sheep.eatGrass(grass);
                }
            }
            for (auto& followSheep : followSheeps) {
                followSheep.move();
                followSheep.findNearestGrass(grasses);
            }
            for (auto& surroundSheep : surroundSheeps) {
                surroundSheep.move();
                surroundSheep.findNearestGrass(grasses);
            }

            auto newEnd = std::remove_if(grasses.begin(), grasses.end(), [](const Grass& grass) {
                return grass.getIsEaten();
                });
            grasses.erase(newEnd, grasses.end());
            totalScore += sheeps[0].getScore();
            bigSheepExp += sheeps[0].getScore();
            sheeps[0].setScore(0);
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
            for (auto& followSheep : followSheeps) {
                followSheep.draw();
            }
            for (auto& surroundSheep : surroundSheeps) {
                surroundSheep.draw();
            }

            settextcolor(WHITE);
            setbkmode(TRANSPARENT);
            LOGFONT globalFont;
            gettextstyle(&globalFont);
            globalFont.lfHeight = 24;
            settextstyle(&globalFont);
            char scoreStr[100];
            sprintf_s(scoreStr, "Level: %d, Exp: %d/%d, 草籽数量: %d, 总分数: %d", bigSheepLevel, bigSheepExp, expToLevelUp, grassSeedCount, totalScore);
            outtextxy(10, 10, scoreStr);

            if (bigSheepExp >= expToLevelUp) {
                if (currentRewards.empty()) {
                    int numRewards = 3 + immortalModeBonus;
                    while (currentRewards.size() < numRewards) {
                        int reward = rand() % 8;
                        bool duplicate = false;
                        for (int r : currentRewards) {
                            if (r == reward) {
                                duplicate = true;
                                break;
                            }
                        }
                        if (!duplicate) {
                            currentRewards.push_back(reward);
                        }
                    }
                }

                int buttonX = width / 2 - 200;
                int buttonY = height / 2 - 50;
                for (int i = 0; i < currentRewards.size(); ++i) {
                    std::string rewardText;
                    switch (currentRewards[i]) {
                    case 0:
                        rewardText = "信号素喷雾";
                        break;
                    case 1:
                        rewardText = "领养协议";
                        break;
                    case 2:
                        rewardText = "gogogo~";
                        break;
                    case 3:
                        rewardText = "勇敢的木";
                        break;
                    case 4:
                        rewardText = "一袋草籽";
                        break;
                    case 5:
                        rewardText = "粪便收集";
                        break;
                    case 6:
                        rewardText = "青草鉴赏家";
                        break;
                    case 7:
                        rewardText = "仙人模式";
                        break;
                    }
                    Button rewardButton(buttonX + i * 150, buttonY, 120, 40, rewardText);
                    rewardButton.draw();
                }
            }
        }
        EndBatchDraw();
        clock_t endTime = clock();
        if (FPS - (endTime - startTime) > 0) {
            Sleep(FPS - (endTime - startTime));
        }
    }
    ~Game() {
        closegraph();
    }
};
int main() {
    Game game;
    while (true) {
        game.handleInput();
        game.update();
        game.draw();
    }
    return 0;
}