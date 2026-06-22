#pragma once
#include <Qpainter>
enum EntityType { TYPE_PLAYER_BALL, TYPE_FOOD, TYPE_VIRUS };
class Ball {
private:
    float x;          // 世界绝对X坐标
    float y;          // 世界绝对Y坐标
    float radius;     // 球体半径（由质量衍生而来）
    float mass;       // 球体质量（核心驱动属性）
    QColor color;
    EntityType type;
public:
    // 构造函数，默认赋予一个初始质量（比如 400.0f，折算半径就是 20.0f）
    Ball(float startX, float startY, EntityType t, float startMass = 400.0f, QColor color = Qt::white);
    ~Ball() = default;

    // update 不需要返回值,并且不是所有的子类有有这个update所以不用纯虚函数
    virtual void update(float deltaTime, float targetX, float targetY){};
    // {}; 空实现,这样调用他的类即使没有重写update也可以继承它
    // 根本报错原因:带有虚函数的类会生成虚函数表(virtual table),表内存放每个虚函数的内存地址,只声明
    // 不实现,编译器找不到 Ball::update的函数实体,虚表内没有发现update函数的实现体,就一定报错,因为编译器认为
    // 既然你写了虚函数,后续就应该要有实现,这里{},空实现,相当于给虚函数表加一个函数实体,虽然这里什么也没做
    //,比如virus,food,需要update的
    // 在单独去实现即可

    // 纯声明：基础属性获取接口
    float getX() const;
    float getY() const;
    float getRadius() const;
    float getMass() const;
    void setMass(float newMass);
    // 核心重构接口：往后外部只需要给球灌注质量
    void addMass(float amount);

    // 边界控制或特殊情况需要的坐标设置接口
    void setPosition(float newX, float newY);
    EntityType getType() const;

    void draw(float cameraX, float cameraY, float screenWidth, float screenHeight,float scale, QPainter& painter);
    // 虽然基类不需要,getcolor,getspeed,setcolor,但是对于基类指向子类的情况我们有必要在基类里面给出一个虚函数或者虚实现
    virtual QColor getColor() const;
    // 1 .类指针只能访问这个类本身的函数,基类指针可以指向子类因为子类里面有基类所有的函数,
    // 但是子类不能指向基类因为基类并不一定有子类所有的函数=>基类指针只能访问基类的所有函数
    // 2 .虚函数只解决同名覆盖的多态问题,解决同一个函数在基类和子类不同版本的问题
    // 虚函数要求函数返回值,函数名,参数都相同,又联想到我们的food,virus,还有controllableball
    // 都有getcolor的函数要求所以这里我们用纯虚函数来解决这个问题,对player的子球谁先画排序的时候发现ball必须要有一个颜色类所以这里还是
    // 给ball一个颜色
    virtual void getSpeed(float &acceptSpeed) const{};
    // 因为并非所有子类都需要去获取speed,而我们的纯虚函数一定要去实现,但对于子类food和virus来说
    // 如果去写一个getspeed就显得画蛇添足了,完全不需要,因此这里不用纯虚函数,上面这个是因为我们的所有子类都需要这个getColor
    // 为什么返回值设置为void,因为如果返回值是 float的类型
    // 就会导致我们必须要在ball类型里面加一个speed,并且我们又不打算在food,virus子类里面赋予它speed的特性
    // 如果在ball里面写speed,绝大多数子类的对象比如food,virus都不需要这个speed,这就是浪费了
    // 所以这里返回空,但是我们要在需要这个speed的子类里面获取这个就必须传一个参数去间接接收它
    virtual void setSpeed(float newspeed) {}; // 修改子类中的speed ,这里的虚实现和上面的update一样作用
private:
    // 内部私有函数：质量改变时，自动重新折算视觉半径
    void recalculateRadius();
};