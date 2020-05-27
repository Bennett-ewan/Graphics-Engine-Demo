#include <stdafx.h>

#include <Components/Collider.h>
#include <Physics/Collision.h>
#include <Components/Transform.h>
#include <Objects/GameObject.h>

using namespace reactphysics3d;

Collider::Collider(::Transform* transform) : Component(ComponentType::cmp_Collider), shape_(), body_(nullptr), shapes_()
{
  DynamicsWorld* world = Collision::Dynamics_World();
  Vector3 pos = Glm_To_React(transform->Translation());
  Quaternion ori = Quaternion::identity();
  rp3d::Transform trans(pos, ori);

  body_ = world->createRigidBody(trans);
  body_->setType(rp3d::BodyType::DYNAMIC);
}

Collider::Collider(Shape shape) : Component(ComponentType::cmp_Collider), shape_(shape), shapes_()
{
  if (shape_ == Shape::Box)
  {
    //rp3d::BoxShape* box_shape = new BoxShape(Glm_To_React(glm::vec3(1.f, 1.f, 1.f)));
    //collider_shape_ = box_shape;
  }
  else if (shape_ == Shape::Sphere)
  {
    //rp3d::SphereShape* sphere_shape = new SphereShape(5.f);
    //collider_shape_ = sphere_shape;
  }
}

Collider::~Collider()
{
  //for (unsigned i = 0; i < shapes_.size(); ++i)
  //{
  //  delete shapes_[i];
  //}
  //shapes_.clear();

}

Collider::Collider(const Collider& other) : Component(ComponentType::cmp_Collider)
{
  *this = other;
}


Component* Collider::Clone() const
{
  return new Collider(*this);
}

void Collider::Init()
{
  DynamicsWorld* world = Collision::Dynamics_World();
  world->destroyRigidBody(body_);

}

void Collider::Tick(float dt)
{
  const rp3d::Transform& transform = body_->getTransform();
  glm::mat4 mtx(1.f);
  transform.getOpenGLMatrix(glm::value_ptr(mtx));
  //Parent()->Get_Component<::Transform>(ComponentType::cmp_Transform)->Matrix(mtx);
}

void Collider::DInit()
{
 
}

void Collider::Desearialize(rapidjson::Document& document)
{
  const rapidjson::Value& coll = document["Collider"];
  assert(coll.IsObject());
  rapidjson::GenericObject collider = coll.GetObject();

  if (collider.HasMember("Shape"))
  {
    std::string shape = collider["Shape"].GetString();
    if (shape == "Box")
    {
      Add_Shape(Shape::Box);
    }
    else if (shape == "Sphere")
    {
      Add_Shape(Shape::Sphere);
    }
  }
}

void Collider::Write(std::ofstream& file)
{

}


void Collider::Read(std::fstream& file)
{

}


void Collider::Add_Shape(Shape shape)
{
  ::Transform *transform = Parent()->Get_Component<::Transform>(ComponentType::cmp_Transform);
  if (shape == Shape::Box)
  {
    const glm::vec3& scale = transform->Scale();
    Vector3 half_extents(scale.x / 2.f, scale.y / 2.f, scale.z / 2.f);
    BoxShape *box = new BoxShape(half_extents);
    shapes_.push_back(box);

    Vector3 pos = Glm_To_React(transform->Translation());
    Quaternion ori = Quaternion::identity();
    rp3d::Transform trans(pos, ori);

    body_->addCollisionShape(box, trans, 5.f);
  }
  else if (shape == Shape::Sphere)
  {

  }
}