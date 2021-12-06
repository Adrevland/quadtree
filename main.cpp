#include <iostream>
#include <vector>
#include <string>
#include <random>

class Vector2d {
public:
    Vector2d();
    Vector2d(double x1, double y1);

    double X;
    double Y;

    Vector2d operator + (const Vector2d& V)const;
    Vector2d operator - (const Vector2d& V)const;
    Vector2d operator / (int i)const;
    double operator * (const Vector2d& V)const;

private:

};

Vector2d::Vector2d(double x1, double y1)
        : X{x1}, Y{y1} {}

Vector2d::Vector2d() {}

Vector2d Vector2d::operator+(const Vector2d &V) const {
    Vector2d U;
    U.X = X + V.X;
    U.Y = Y + V.Y;
    return U;
}

Vector2d Vector2d::operator/(int i) const {
    Vector2d U;
    U.X = X / i;
    U.Y = Y / i;
    return U;
}

double Vector2d::operator*(const Vector2d &V) const {
    double len;
    Vector2d U;
    U.X = abs(X - V.X);
    U.Y = abs(Y - V.Y);

    len = abs((sqrt(pow(U.X,2)+pow(U.Y,2))));
    return len;
}


Vector2d Vector2d::operator-(const Vector2d &V) const {
    Vector2d U;
    U.X = X - V.X;
    U.Y = Y - V.Y;
    return U;
}
class GameObject {
public:
    GameObject();
    GameObject(const Vector2d& Position);
    GameObject(const Vector2d& Position, std::string Name);
    Vector2d GetPosition() const;
    std::string GetName() const;
private:
    Vector2d m_Position;
    std::string m_Name;
};
GameObject::GameObject() {}

GameObject::GameObject(const Vector2d &Position, std::string Name)
        :m_Position{Position},m_Name{Name}{}

std::string GameObject::GetName() const {
    return m_Name;
}

Vector2d GameObject::GetPosition() const {
    return m_Position;
}

GameObject::GameObject(const Vector2d &Position) :m_Position{Position}, m_Name{"NoName"}{
}

class QuadNode {
public:
    QuadNode();
    QuadNode(const Vector2d &v_1,const Vector2d &v_2,const Vector2d &v_3,const Vector2d &v_4);
    void SubDivide(int n);
    void PrintAll();
    void MaxObjectsQuad(int n);
    std::pair<QuadNode*, bool> Insert(const GameObject &GameObject);
    QuadNode* Find(const Vector2d& p);

    Vector2d m_A;
    Vector2d m_B;
    Vector2d m_C;
    Vector2d m_D;

    QuadNode* m_SV;
    QuadNode* m_SO;
    QuadNode* m_NV;
    QuadNode* m_NO;

    std::vector<GameObject> m_GameObjects;
    bool isLeaf() const;
};

bool QuadNode::isLeaf() const {
    if(m_SO)
        return false;
    else
        return true;
}

QuadNode::QuadNode() {}

QuadNode::QuadNode(const Vector2d &v_1, const Vector2d &v_2, const Vector2d &v_3, const Vector2d &v_4)
        :m_A{v_1},m_B{v_2},m_C{v_3},m_D{v_4},
         m_SV{nullptr},m_SO{nullptr},m_NV{nullptr},m_NO{nullptr}
{}

void QuadNode::SubDivide(int n) {
    if(n>0){
        Vector2d AB{(m_A+m_B)/2};
        Vector2d BC{(m_B+m_C)/2};
        Vector2d CD{(m_C+m_D)/2};
        Vector2d AD{(m_D+m_A)/2};
        Vector2d M{(m_A+m_C)/2};

        m_SV = new QuadNode{m_A,AB,M,AD};
        m_SO = new QuadNode{AB,m_B,BC,M};
        m_NO = new QuadNode{M,BC,m_C,CD};
        m_NV = new QuadNode{AD,M,CD,m_D};

        //recursion
        n--;
        m_SV->SubDivide(n);
        m_SO->SubDivide(n);
        m_NO->SubDivide(n);
        m_NV->SubDivide(n);
    }
}


std::pair<QuadNode*, bool> QuadNode::Insert(const GameObject &GameObject) {

    QuadNode* Node{ Find(GameObject.GetPosition())};
    if(Node){
        Node->m_GameObjects.emplace_back(GameObject);
        return {Node, true};
    }
    else
        return {nullptr, false};
}

QuadNode *QuadNode::Find(const Vector2d &p) {
    if(isLeaf()){
        return this;
    }
    else{
        Vector2d m = (m_A+m_C)/2;
        if(p.Y <= m.Y)
            if(p.X <= m.X)
                return m_SV->Find(p);
            else
                return m_SO->Find(p);
        else
        if(p.X < m.X)
            return m_NV->Find(p);
        else
            return m_NO ->Find(p);
    }
    return nullptr;
}


void QuadNode::PrintAll() {

    if(m_SO)
        m_SO->PrintAll();
    if(m_SV)
        m_SV->PrintAll();
    if(m_NO)
        m_NO->PrintAll();
    if(m_NV)
        m_NV->PrintAll();

    //print quad
    std::cout << "Quad : ";
    std::cout << "(" <<m_A.X << ", " << m_A.Y << "), ";
    std::cout << "(" <<m_B.X << ", " << m_B.Y << "), ";
    std::cout << "(" <<m_C.X << ", " << m_C.Y << "), ";
    std::cout << "(" <<m_D.X << ", " << m_D.Y << "), ";
    std::cout <<std::endl;
    for(auto obj: this->m_GameObjects){
        std::cout << "Object location : "<< "("<< obj.GetPosition().X <<", " <<obj.GetPosition().Y << " )\n";
    }
    std::cout << "\n\n";
}

void QuadNode::MaxObjectsQuad(int n) {

    //subdivide once if it is more game objects than n
    if(m_GameObjects.size() > n){
        SubDivide(1);

        for(auto obj: m_GameObjects){ // puts every object in new quad
            auto temp  = Find(obj.GetPosition());
            temp->Insert(obj);
        }
        m_GameObjects.clear(); // empty whole array
    }
    else return;
    /* blir stuck i evig loop. trenger litt tenking her
    if(m_SO && m_SO->m_GameObjects.size() > n) m_SO->MaxObjectsQuad(n);
    if(m_NO && m_NO->m_GameObjects.size() > n) m_NO->MaxObjectsQuad(n);
    if(m_SV && m_SV->m_GameObjects.size() > n) m_SV->MaxObjectsQuad(n);
    if(m_NV && m_NV->m_GameObjects.size() > n) m_NV->MaxObjectsQuad(n);
     */
}


int main() {


    Vector2d NV{-2,2};
    Vector2d NO{2,2};
    Vector2d SV{2,-2};
    Vector2d SO{2,-2};
    auto RootNode = new QuadNode{NV,NO,SV,SO};

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> r2_2(-2.0, 2.0);
    std::uniform_real_distribution<> r0_2(-2.0, 2.0);
    // first 20 game objects.
    for(int i= 0; i < 20; i++){

        //-2-2
        double x = r2_2(gen);
        double y = r2_2(gen);
        RootNode->Insert( GameObject{Vector2d{x,y}});
        //0-2
        double x2 = r0_2(gen);
        double y2 = r0_2(gen);
        RootNode->Insert( GameObject{Vector2d{x2,y2}});
    }

    int n = 5; // max n objects in each quad
    RootNode->MaxObjectsQuad(n); // subdivides and distribute objects

    RootNode->PrintAll();

    return 0;
}
