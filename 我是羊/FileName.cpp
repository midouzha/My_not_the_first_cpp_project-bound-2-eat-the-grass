//#include <string>
//#include <vector>
//#include <conio.h>
//#include <easyx.h>
//#include <iostream>
//#include <cmath>
//#include <cstdlib>
//#include <ctime>
//#include <fstream>
//using namespace std;
//const float pix = 40;
//const int width = 800;
//const int height = 640;
//const clock_t FPS = 1000 / 60;
//const int EAT_DISTANCE = 10; // 设置羊距离草若干像素就可以吃草
//
//// 游戏状态枚举
//enum GameState {
//    STATE_MENU,
//    STATE_RECT_CHALLENGE,
//    STATE_FAIR_SHARE,
//    STATE_END_GAME
//};
//
//// 草类
//class Grass {
//private:
//    int x, y;
//    bool isEaten;
//    IMAGE* image;
//    IMAGE* mask;
//    int sheepEatingCount;
//public:
//    Grass(int _x, int _y, IMAGE* img, IMAGE* msk) : x(_x), y(_y), isEaten(false), image(img), mask(msk), sheepEatingCount(0) {}
//
//    void draw() {
//        if (!isEaten) {
//            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, mask, NOTSRCERASE);
//            putimage(x - image->getwidth() / 2, y - image->getheight() / 2, image, SRCINVERT);
//        }
//    }
//
//    bool getIsEaten() const {
//        return isEaten;
//    }
//
//    void setIsEaten(bool eaten) {
//        isEaten = eaten;
//    }
//
//    int getSheepEatingCount() const {
//        return sheepEatingCount;
//    }
//
//    void incrementSheepEatingCount() {
//        sheepEatingCount++;
//    }
//
//    void resetSheepEatingCount() {
//        sheepEatingCount = 0;
//    }
//
//    int getX() const {
//        return x;
//    }
//
//    int getY() const {
//        return y;
//    }
//};
//
//// 羊类
//class Sheep {
//private:
//    int x, y;
//    int targetX, targetY;
//    bool isMoving;
//    int MOVE_SPEED;
//    IMAGE* image;
//    IMAGE* mask;
//    bool isEating;
//    int eatTimeLeft;
//    int currentTargetGrassIndex;
//    int grassEatenCount;
//public:
//    Sheep(int _x, int _y, int speed, IMAGE* img, IMAGE* msk) : x(_x), y(_y), targetX(_x), targetY(_y), isMoving(false), MOVE_SPEED(speed), image(img), mask(msk), isEating(false), eatTimeLeft(100), currentTargetGrassIndex(-1), grassEatenCount(0) {}
//
//    void draw() {
//        putimage(x, y, mask, NOTSRCERASE);
//        putimage(x, y, image, SRCINVERT);
//    }
//
//    void findNearestGrass(const std::vector<Grass>& grasses) {
//        int minDistance = INT_MAX;
//        int nearestGrassIndex = -1;
//        for (size_t i = 0; i < grasses.size(); ++i) {
//            if (!grasses[i].getIsEaten()) {
//                int dx = grasses[i].getX() - x;
//                int dy = grasses[i].getY() - y;
//                int distance = dx * dx + dy * dy;
//                if (distance < minDistance) {
//                    minDistance = distance;
//                    nearestGrassIndex = static_cast<int>(i);
//                }
//            }
//        }
//        if (nearestGrassIndex != -1) {
//            targetX = grasses[nearestGrassIndex].getX();
//            targetY = grasses[nearestGrassIndex].getY();
//            isMoving = true;
//            currentTargetGrassIndex = nearestGrassIndex;
//        }
//    }
//
//    void move(std::vector<Grass>& grasses) {
//        if (isMoving) {
//            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size() && grasses[currentTargetGrassIndex].getIsEaten()) {
//                isMoving = false;
//                currentTargetGrassIndex = -1;
//                findNearestGrass(grasses);
//                return;
//            }
//            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size()) {
//                int dx = targetX - x;
//                int dy = targetY - y;
//                float distance = static_cast<float>(sqrt(static_cast<double>(dx * dx + dy * dy)));
//
//                if (distance > MOVE_SPEED && distance > EAT_DISTANCE) {
//                    float ratio = MOVE_SPEED / distance;
//                    x += static_cast<int>(dx * ratio);
//                    y += static_cast<int>(dy * ratio);
//                }
//                else {
//                    isMoving = false;
//                    isEating = true;
//                    if (currentTargetGrassIndex != -1) {
//                        grasses[currentTargetGrassIndex].incrementSheepEatingCount();
//                    }
//                }
//            }
//        }
//    }
//
//    void eat(std::vector<Grass>& grasses) {
//        if (isEating) {
//            if (currentTargetGrassIndex >= 0 && static_cast<size_t>(currentTargetGrassIndex) < grasses.size()) {
//                Grass& currentGrass = grasses[currentTargetGrassIndex];
//                for (int i = 0; i < currentGrass.getSheepEatingCount(); ++i) {
//                    if (eatTimeLeft > 0) {
//                        eatTimeLeft--;
//                    }
//                }
//                if (eatTimeLeft <= 0) {
//                    currentGrass.setIsEaten(true);
//                    isEating = false;
//                    currentTargetGrassIndex = -1;
//                    currentGrass.resetSheepEatingCount();
//                    grassEatenCount++;
//                    findNearestGrass(grasses);
//                }
//            }
//        }
//    }
//
//    bool isInsideRect(int left, int top, int right, int bottom) {
//        return x >= left && x <= right && y >= top && y <= bottom;
//    }
//
//    int getGrassEatenCount() const {
//        return grassEatenCount;
//    }
//};
//
//// 矩形类
//class Rectangle1 {
//private:
//    int left, top, right, bottom;
//public:
//    Rectangle1(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
//
//    void draw() {
//        setlinestyle(PS_SOLID,3);
//        setlinecolor(BROWN);
//        rectangle(left, top, right, bottom);
//    }
//
//    int getLeft() const { return left; }
//    int getTop() const { return top; }
//    int getRight() const { return right; }
//    int getBottom() const { return bottom; }
//};
//
//// 按钮类
//class Button {
//private:
//    int left, top, right, bottom;
//    
//    bool isHovered;
//public:
//    string text;
//    Button(int l, int t, int r, int b, const std::string& txt) : left(l), top(t), right(r), bottom(b), text(txt), isHovered(false) {}
//
//    bool isClicked(int x, int y) {
//        return x >= left && x <= right && y >= top && y <= bottom;
//    }
//
//    bool isHover(int x, int y) {
//        isHovered = x >= left && x <= right && y >= top && y <= bottom;
//        return isHovered;
//    }
//
//    void draw() {
//        setfillcolor(isHovered ? LIGHTGRAY : WHITE);
//        setlinecolor(BLACK);
//        fillroundrect(left, top, right, bottom, 10, 10);
//        settextcolor(BLACK);
//        outtextxy(left + (right - left - textwidth(text.c_str())) / 2, top + (bottom - top - textheight(text.c_str())) / 2, text.c_str());
//    }
//};
//
//// 游戏类
//class Game {
//private:
//    GameState state;
//    float seed;
//    IMAGE start, bk1, sheep_small_img, sheep_small_mask, sheep_big_img, sheep_big_mask, grassImage, grassMask;
//    std::vector<Grass> grasses;
//    std::vector<Sheep> sheeps;
//    std::vector<Rectangle1> rectangles;
//    std::vector<Button> buttons;
//    int round;
//    int bestRound;
//    int bigSheepEaten;
//    int smallSheepEaten;
//    int score;
//    int bestScore;
//    double rectDistanceRatio;
//
//    void loadData() {
//        std::ifstream file("game_data.txt");
//        if (file.is_open()) {
//            file >> bestRound >> bestScore;
//            file.close();
//        }
//        else {
//            bestRound = 0;
//            bestScore = 0;
//        }
//    }
//
//    void saveData() {
//        std::ofstream file("game_data.txt");
//        if (file.is_open()) {
//            file << bestRound << " " << bestScore;
//            file.close();
//        }
//    }
//
//    void generateRectangles() {
//        rectangles.clear();
//        int diagonal = static_cast<int>(sqrt(width * width + height * height));
//        int minDistance = static_cast<int>(diagonal * 0.1);
//        int currentDistance = static_cast<int>(diagonal * rectDistanceRatio);
//        rectDistanceRatio = max(0.1, rectDistanceRatio - 0.05);
//
//        int bigRectSize = 120;
//        int smallRectSize = 80;
//
//        int left1 = rand() % (width - bigRectSize);
//        int top1 = rand() % (height - bigRectSize);
//        rectangles.emplace_back(left1, top1, left1 + bigRectSize, top1 + bigRectSize);
//
//        int left2, top2;
//        do {
//            left2 = rand() % (width - smallRectSize);
//            top2 = rand() % (height - smallRectSize);
//            int centerX1 = left1 + bigRectSize / 2;
//            int centerY1 = top1 + bigRectSize / 2;
//            int centerX2 = left2 + smallRectSize / 2;
//            int centerY2 = top2 + smallRectSize / 2;
//            int distance = static_cast<int>(sqrt((centerX1 - centerX2) * (centerX1 - centerX2) + (centerY1 - centerY2) * (centerY1 - centerY2)));
//        } while (sqrt((left1 - left2) * (left1 - left2) + (top1 - top2) * (top1 - top2)) < currentDistance);
//        rectangles.emplace_back(left2, top2, left2 + smallRectSize, top2 + smallRectSize);
//    }
//
//    void handleMenuInput(int x, int y, bool clicked) {
//        for (auto& button : buttons) {
//            button.isHover(x, y);
//            if (clicked && button.isClicked(x, y)) {
//                if (button.text == "矩形挑战") {
//                    state = STATE_RECT_CHALLENGE;
//                    round = 0;
//                    rectDistanceRatio = 0.5;
//                    generateRectangles();
//                    grasses.clear();
//                    for (auto& sheep : sheeps) {
//                        sheep.move(grasses);
//                        sheep.findNearestGrass(grasses);
//                    }
//                }
//                else if (button.text == "孔融让梨") {
//                    state = STATE_FAIR_SHARE;
//                    bigSheepEaten = 0;
//                    smallSheepEaten = 0;
//                    score = 0;
//                    grasses.clear();
//                    for (auto& sheep : sheeps) {
//                        sheep.move(grasses);
//                        sheep.findNearestGrass(grasses);
//                    }
//                }
//            }
//        }
//    }
//
//    void handleRectChallengeInput(int x, int y, bool clicked) {
//        if (clicked) {
//            if (round == 0) {
//                grasses.emplace_back(x, y, &grassImage, &grassMask);
//            }
//            for (auto& button : buttons) {
//                button.isHover(x, y);
//                if (button.isClicked(x, y)) {
//                    if (button.text == "重新开始") {
//                        state = STATE_RECT_CHALLENGE;
//                        round = 0;
//                        rectDistanceRatio = 0.5;
//                        generateRectangles();
//                        grasses.clear();
//                        for (auto& sheep : sheeps) {
//                            sheep.move(grasses);
//                            sheep.findNearestGrass(grasses);
//                        }
//                    }
//                    else if (button.text == "返回主菜单") {
//                        state = STATE_MENU;
//                    }
//                }
//            }
//        }
//    }
//
//    void handleFairShareInput(int x, int y, bool clicked) {
//        if (clicked) {
//            grasses.emplace_back(x, y, &grassImage, &grassMask);
//            for (auto& button : buttons) {
//                button.isHover(x, y);
//                if (button.isClicked(x, y)) {
//                    if (button.text == "结束本局") {
//                        score = bigSheepEaten * bigSheepEaten / (smallSheepEaten + 1);
//                        if (score > bestScore) {
//                            bestScore = score;
//                            saveData();
//                        }
//                        state = STATE_MENU;
//                    }
//                }
//            }
//        }
//    }
//
//    void updateRectChallenge() {
//        if (!grasses.empty()) {
//            for (auto& sheep : sheeps) {
//                sheep.findNearestGrass(grasses);
//            }
//        }
//        for (auto& sheep : sheeps) {
//            sheep.move(grasses);
//        }
//        for (auto& sheep : sheeps) {
//            sheep.eat(grasses);
//        }
//
//        auto newEnd = std::remove_if(grasses.begin(), grasses.end(), [](const Grass& grass) {
//            return grass.getIsEaten();
//            });
//        grasses.erase(newEnd, grasses.end());
//
//        if (!grasses.empty()) {
//            for (auto& sheep : sheeps) {
//                sheep.findNearestGrass(grasses);
//            }
//        }
//
//        if (sheeps[0].isInsideRect(rectangles[0].getLeft(), rectangles[0].getTop(), rectangles[0].getRight(), rectangles[0].getBottom()) &&
//            sheeps[1].isInsideRect(rectangles[1].getLeft(), rectangles[1].getTop(), rectangles[1].getRight(), rectangles[1].getBottom())) {
//            round++;
//            if (round > bestRound) {
//                bestRound = round;
//                saveData();
//            }
//            rectDistanceRatio = max(0.1, rectDistanceRatio - 0.05);
//            generateRectangles();
//            grasses.clear();
//        }
//        else if (grasses.empty()) {
//            state = STATE_END_GAME;
//            buttons.clear();
//            buttons.emplace_back(width / 2 - 100, height / 2 + 50, width / 2 + 100, height / 2 + 90, "重新开始");
//            buttons.emplace_back(width / 2 - 100, height / 2 + 110, width / 2 + 100, height / 2 + 150, "返回主菜单");
//        }
//    }
//
//    void updateFairShare() {
//        if (!grasses.empty()) {
//            for (auto& sheep : sheeps) {
//                sheep.findNearestGrass(grasses);
//            }
//        }
//        for (auto& sheep : sheeps) {
//            sheep.move(grasses);
//        }
//        for (auto& sheep : sheeps) {
//            sheep.eat(grasses);
//        }
//
//        auto newEnd = std::remove_if(grasses.begin(), grasses.end(), [](const Grass& grass) {
//            return grass.getIsEaten();
//            });
//        grasses.erase(newEnd, grasses.end());
//
//        bigSheepEaten = sheeps[0].getGrassEatenCount();
//        smallSheepEaten = sheeps[1].getGrassEatenCount();
//        score = bigSheepEaten * bigSheepEaten / (smallSheepEaten + 1);
//    }
//
//    void drawMenu() {
//        putimage(0, 0, &start);
//        buttons.clear();
//        buttons.emplace_back(width / 2 - 100, height / 2 - 50, width / 2 + 100, height / 2 - 10, "矩形挑战");
//        buttons.emplace_back(width / 2 - 100, height / 2 + 10, width / 2 + 100, height / 2 + 50, "孔融让梨");
//        for (auto& button : buttons) {
//            button.draw();
//        }
//    }
//
//    void drawRectChallenge() {
//        cleardevice();
//        putimage(0, 0, &bk1);
//        for (auto& grass : grasses) {
//            grass.draw();
//        }
//        for (auto& sheep : sheeps) {
//            sheep.draw();
//        }
//        for (auto& rect : rectangles) {
//            rect.draw();
//        }
//
//        settextcolor(WHITE);
//        char info[100];
//        sprintf_s(info, "游戏模式: 矩形挑战  回合数: %d  最好成绩: %d", round, bestRound);
//        outtextxy(10, 10, info);
//    }
//
//    void drawFairShare() {
//        cleardevice();
//        putimage(0, 0, &bk1);
//        for (auto& grass : grasses) {
//            grass.draw();
//        }
//        for (auto& sheep : sheeps) {
//            sheep.draw();
//        }
//
//        settextcolor(WHITE);
//        char info[200];
//        sprintf_s(info, "游戏模式: 孔融让梨  当前得分: %d  大羊吃草数: %d  小羊吃草数: %d  历史最高分: %d", score, bigSheepEaten, smallSheepEaten, bestScore);
//        outtextxy(10, 10, info);
//
//        buttons.clear();
//        buttons.emplace_back(width - 120, 10, width - 20, 50, "结束本局");
//        for (auto& button : buttons) {
//            button.draw();
//        }
//    }
//
//    void drawEndGame() {
//        cleardevice();
//        putimage(0, 0, &bk1);
//        settextcolor(RED);
//        char gameOverMsg[100];
//        sprintf_s(gameOverMsg, "游戏失败！最高连胜回合数: %d", bestRound);
//        outtextxy(width / 2 - textwidth(gameOverMsg) / 2, height / 2, gameOverMsg);
//        for (auto& button : buttons) {
//            button.draw();
//        }
//    }
//
//public:
//    Game() : state(STATE_MENU), seed(300), round(0), bestRound(0), bigSheepEaten(0), smallSheepEaten(0), score(0), bestScore(0), rectDistanceRatio(0.5) {
//        srand(static_cast<unsigned int>(time(nullptr)));
//        initgraph(width, height);
//
//        // 使用相对路径加载图片
//        loadimage(&start, _T("assets/start.png"));
//        loadimage(&bk1, _T("assets/bg1.png"));
//        loadimage(&sheep_small_img, _T("assets/sheeps3cm.png"));
//        loadimage(&sheep_small_mask, _T("assets/sheeps3cm_mask.png"));
//        loadimage(&sheep_big_img, _T("assets/sheeps5.png"));
//        loadimage(&sheep_big_mask, _T("assets/sheeps5_mask.png"));
//        loadimage(&grassImage, _T("assets/grass1.png"));
//        loadimage(&grassMask, _T("assets/grass1_mask.png"));
//
//        // 大羊速度快，小羊速度慢
//        sheeps.emplace_back(width / 3, height / 2, 5, &sheep_big_img, &sheep_big_mask);
//        sheeps.emplace_back(width * 2 / 3, height / 4, 2, &sheep_small_img, &sheep_small_mask);
//
//        loadData();
//    }
//
//    void run() {
//        int flag = 0;
//        while (true) {
//            if (seed >= 900) flag = 1;
//            if (seed <= 300) flag = 0;
//            flag ? seed -= pix : seed += pix;
//
//            ExMessage msg;
//            while (peekmessage(&msg, EX_MOUSE)) {
//                if (msg.message == WM_MOUSEMOVE) {
//                    switch (state) {
//                    case STATE_MENU:
//                        handleMenuInput(msg.x, msg.y, false);
//                        break;
//                    case STATE_RECT_CHALLENGE:
//                        handleRectChallengeInput(msg.x, msg.y, false);
//                        break;
//                    case STATE_FAIR_SHARE:
//                        handleFairShareInput(msg.x, msg.y, false);
//                        break;
//                    }
//                }
//                else if (msg.message == WM_LBUTTONDOWN) {
//                    switch (state) {
//                    case STATE_MENU:
//                        handleMenuInput(msg.x, msg.y, true);
//                        break;
//                    case STATE_RECT_CHALLENGE:
//                        handleRectChallengeInput(msg.x, msg.y, true);
//                        break;
//                    case STATE_FAIR_SHARE:
//                        handleFairShareInput(msg.x, msg.y, true);
//                        break;
//                    }
//                }
//            }
//
//            switch (state) {
//            case STATE_MENU:
//                drawMenu();
//                break;
//            case STATE_RECT_CHALLENGE:
//                updateRectChallenge();
//                drawRectChallenge();
//                break;
//            case STATE_FAIR_SHARE:
//                updateFairShare();
//                drawFairShare();
//                break;
//            case STATE_END_GAME:
//                drawEndGame();
//                break;
//            }
//
//            Sleep(FPS);
//        }
//    }
//};
//
//int main() {
//    Game game;
//    game.run();
//    system("pause");
//    return 0;
//}