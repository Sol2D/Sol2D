// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <Sol2D/World/Scene.h>
#include <Sol2D/World/UserData.h>
#include <Sol2D/World/AStar.h>
#include <Sol2D/Tiles/Tmx.h>
#include <Sol2D/Utils/Observable.h>
#include <unordered_set>

using namespace Sol2D;
using namespace Sol2D::World;
using namespace Sol2D::Tiles;
using namespace Sol2D::Utils;

namespace {

void initBodyPhysics(b2BodyDef & _b2_def, const BodyPhysicsDefinition & _physics)
{
    if(_physics.linear_damping.has_value())
        _b2_def.linearDamping = _physics.linear_damping.value();
    if(_physics.angular_damping.has_value())
        _b2_def.angularDamping = _physics.angular_damping.value();
    _b2_def.fixedRotation = _physics.fixed_rotation;
}

void initShapePhysics(b2ShapeDef & _b2_shape_def, const BodyShapePhysicsDefinition & _physics)
{
    _b2_shape_def.isSensor = _physics.is_sensor;
    _b2_shape_def.enablePreSolveEvents = _physics.is_pre_solve_enabled;
    if(_physics.density.has_value())
        _b2_shape_def.density = _physics.density.value();
    if(_physics.restitution.has_value())
        _b2_shape_def.restitution = _physics.restitution.value();
    if(_physics.friction.has_value())
        _b2_shape_def.friction = _physics.friction.value();
}

constexpr SDL_FColor gc_object_debug_color = { .r = 1.0f, .g = .08f, .b = .0f, .a = 1.0f }; // TODO: from config

} // namespace

class Scene::BodyShapeCreator
{
public:
    explicit BodyShapeCreator(Scene & _scene, Body & _body, const b2BodyId & _b2_body_id, const std::string & _key);
    void operator ()(const BodyPolygonDefinition & _polygon);
    void operator ()(const BodyRectDefinition & _rect);
    void operator ()(const BodyCircleDefinition & _circle);
    void operator ()(const BodyCapsuleShapeDefinition & _capsule);

private:
    template<BodyShapeType shape_type>
    b2ShapeDef createBox2dShapeDef(const BodyBasicShapeDefinition<shape_type> & _def) const;
    template<BodyShapeType shape_type>
    BodyShape & createShape(const BodyBasicShapeDefinition<shape_type> & _def) const;

private:
    Scene & mr_scene;
    Body & mr_body;
    const b2BodyId & mr_b2_body_id;
    const std::string & mr_key;
};

Scene::BodyShapeCreator::BodyShapeCreator(
    Scene & _scene,
    Body & _body,
    const b2BodyId & _b2_body_id,
    const std::string & _key
) :
    mr_scene(_scene),
    mr_body(_body),
    mr_b2_body_id(_b2_body_id),
    mr_key(_key)
{
}

void Scene::BodyShapeCreator::operator ()(const BodyPolygonDefinition & _capsule)
{
    if(_capsule.points.size() < 3 || _capsule.points.size() > B2_MAX_POLYGON_VERTICES)
        return; // TODO: log
    std::vector<b2Vec2> shape_points(_capsule.points.size());
    for(size_t i = 0; i < _capsule.points.size(); ++i)
    {
        shape_points[i].x = mr_scene.graphicalToPhysical(_capsule.points[i].x);
        shape_points[i].y = mr_scene.graphicalToPhysical(_capsule.points[i].y);
    }
    b2Hull b2_hull = b2ComputeHull(shape_points.data(), shape_points.size());
    b2Polygon b2_polygon = b2MakePolygon(&b2_hull, .0f);
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_capsule);
    b2ShapeId b2_shape_id = b2CreatePolygonShape(mr_b2_body_id, &b2_shape_def, &b2_polygon);
    BodyShape & body_shape = createShape(_capsule);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

template<BodyShapeType shape_type>
inline b2ShapeDef Scene::BodyShapeCreator::createBox2dShapeDef(const BodyBasicShapeDefinition<shape_type> & _def) const
{
    b2ShapeDef b2_shape_def = b2DefaultShapeDef();
    initShapePhysics(b2_shape_def, _def.physics);
    return b2_shape_def;
}

template<BodyShapeType shape_type>
BodyShape & Scene::BodyShapeCreator::createShape(const BodyBasicShapeDefinition<shape_type> & _def) const
{
    BodyShape & body_shape = mr_body.createShape(mr_key);
    for(const auto & graphics_kv : _def.graphics)
    {
        body_shape.addGraphics(
            mr_scene.mr_renderer,
            makePreHashedKey(graphics_kv.first),
            graphics_kv.second);
    }
    return body_shape;
}

void Scene::BodyShapeCreator::operator ()(const BodyRectDefinition & _rect)
{
    const float half_w = mr_scene.graphicalToPhysical(_rect.w) / 2.0f;
    const float half_h = mr_scene.graphicalToPhysical(_rect.h) / 2.0f;
    b2Polygon b2_polygon = b2MakeOffsetBox(
        half_w,
        half_h,
        {
            .x = (_rect.x ? mr_scene.graphicalToPhysical(_rect.x) : .0f) + half_w,
            .y = (_rect.y ?  mr_scene.graphicalToPhysical(_rect.y) : .0f) + half_h
        },
        b2Rot_identity);
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_rect);
    b2ShapeId b2_shape_id = b2CreatePolygonShape(mr_b2_body_id, &b2_shape_def, &b2_polygon);
    BodyShape & body_shape = createShape(_rect);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

void Scene::BodyShapeCreator::operator ()(const BodyCircleDefinition & _circle)
{
    b2Circle b2_circle
    {
        .center =
        {
            .x = mr_scene.graphicalToPhysical(_circle.center.x),
            .y = mr_scene.graphicalToPhysical(_circle.center.y)
        },
        .radius = mr_scene.graphicalToPhysical(_circle.radius)
    };
    if(b2_circle.radius <= .0f)
        return; // TODO: log
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_circle);
    b2ShapeId b2_shape_id = b2CreateCircleShape(mr_b2_body_id, &b2_shape_def, &b2_circle);
    BodyShape & body_shape = createShape(_circle);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

void Scene::BodyShapeCreator::operator ()(const BodyCapsuleShapeDefinition & _capsule)
{
    b2Capsule b2_capsule
    {
        .center1 = b2Vec2
        {
            .x = mr_scene.graphicalToPhysical(_capsule.center1.x),
            .y = mr_scene.graphicalToPhysical(_capsule.center1.y)
        },
        .center2 = b2Vec2
        {
            .x = mr_scene.graphicalToPhysical(_capsule.center2.x),
            .y = mr_scene.graphicalToPhysical(_capsule.center2.y)
        },
        .radius = mr_scene.graphicalToPhysical(_capsule.radius)
    };
    if(b2_capsule.radius <= .0f)
        return; // TODO: log
    b2ShapeDef b2_shape_def = createBox2dShapeDef(_capsule);
    b2ShapeId b2_shape_id = b2CreateCapsuleShape(mr_b2_body_id, &b2_shape_def, &b2_capsule);
    BodyShape & body_shape = createShape(_capsule);
    b2Shape_SetUserData(b2_shape_id, &body_shape);
}

Scene::Scene(const SceneOptions & _options, const Workspace & _workspace, Renderer & _renderer) :
    mr_workspace(_workspace),
    mr_renderer(_renderer),
    m_world_offset{.0f, .0f},
    m_meters_per_pixel(_options.meters_per_pixel),
    m_followed_body_id(b2_nullBodyId),
    mp_box2d_debug_draw(nullptr)
{
    if(m_meters_per_pixel <= .0f)
        m_meters_per_pixel = SceneOptions::default_meters_per_pixel;
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = toBox2D(_options.gravity);
    m_b2_world_id = b2CreateWorld(&world_def);
    b2World_SetPreSolveCallback(m_b2_world_id, &Scene::box2dPreSolveContact, this);
    if(_workspace.isDebugRenderingEnabled())
    {
        mp_box2d_debug_draw = new Box2dDebugDraw(
            mr_renderer,
            m_b2_world_id,
            [this](float __x, float __y)
            {
                return toAbsoluteCoords(physicalToGraphical(__x), physicalToGraphical(__y));
            },
            [this](float __len)
            {
                return physicalToGraphical(__len);
            }
        );
    }
}

Scene::~Scene()
{
    deinitializeTileMap();
    b2DestroyWorld(m_b2_world_id);
    delete mp_box2d_debug_draw;
}

void Scene::deinitializeTileMap()
{

    for(auto it = m_bodies.begin(); it != m_bodies.end(); it = m_bodies.begin())
        destroyBody(it->first);
    m_bodies.clear();
    m_joints.clear();
    m_tile_heap_ptr.reset();
    m_object_heap_ptr.reset();
    m_tile_map_ptr.reset();
    m_followed_body_id = b2_nullBodyId;
}

void Scene::setGravity(const SDL_FPoint & _vector)
{
    m_defers.getQueue().enqueueAction([this, _vector]() {
        b2World_SetGravity(m_b2_world_id, toBox2D(_vector)); // TODO: scale factor?
    });
}

uint64_t Scene::createBody(const SDL_FPoint & _position, const BodyDefinition & _definition)
{
    b2BodyDef b2_body_def = b2DefaultBodyDef();
    b2_body_def.type = mapBodyType(_definition.type);
    b2_body_def.position = { .x = _position.x, .y = _position.y };
    initBodyPhysics(b2_body_def, _definition.physics);
    b2BodyId b2_body_id = b2CreateBody(m_b2_world_id, &b2_body_def);
    Body * body = new Body(b2_body_id, m_defers.getQueue());
    b2Body_SetUserData(b2_body_id, body);
    m_bodies.insert(std::make_pair(body->getGid(), b2_body_id));
    for(const auto & shape_kv : _definition.shapes)
    {
        BodyShapeCreator visitor(*this, *body, b2_body_id, shape_kv.first);
        std::visit(visitor, shape_kv.second);
    }
    return body->getGid();
}

void Scene::createBodiesFromMapObjects(const std::string & _class, const BodyOptions & _body_options)
{
    b2BodyType body_type = mapBodyType(_body_options.type);
    m_object_heap_ptr->forEachObject([&](const TileMapObject & __map_object) {
        if(__map_object.getClass() != _class) return;
        b2BodyDef b2_body_def = b2DefaultBodyDef();
        b2_body_def.type = body_type;
        initBodyPhysics(b2_body_def, _body_options.body_physics);
        b2_body_def.position =
        {
            .x = graphicalToPhysical(__map_object.getPosition().x),
            .y = graphicalToPhysical(__map_object.getPosition().y)
        };
        b2BodyId b2_body_id = b2CreateBody(m_b2_world_id, &b2_body_def);
        Body * body = new Body(b2_body_id, m_defers.getQueue());
        b2Body_SetUserData(b2_body_id, body);

        m_bodies.insert(std::make_pair(body->getGid(), b2_body_id));
        b2ShapeDef b2_shape_def = b2DefaultShapeDef();
        initShapePhysics(b2_shape_def, _body_options.shape_physics);

        switch(__map_object.getObjectType())
        {
        case TileMapObjectType::Polygon:
        {
            const TileMapPolygon * polygon = static_cast<const TileMapPolygon *>(&__map_object);
            const std::vector<SDL_FPoint> & points = polygon->getPoints();
            if(points.size() < 3 || points.size() > B2_MAX_POLYGON_VERTICES)
                break;
            std::vector<b2Vec2> shape_points(points.size());
            for(size_t i = 0; i < points.size(); ++i)
            {
                shape_points[i].x = graphicalToPhysical(points[i].x);
                shape_points[i].y = graphicalToPhysical(points[i].y);
            }
            b2Hull b2_hull = b2ComputeHull(shape_points.data(), shape_points.size());
            b2Polygon b2_polygon = b2MakePolygon(&b2_hull, .0f);
            b2ShapeId b2_shape_id = b2CreatePolygonShape(b2_body_id, &b2_shape_def, &b2_polygon);
            BodyShape * body_shape = &body->createShape(_body_options.shape_key.value_or(_class), polygon->getId());
            b2Shape_SetUserData(b2_shape_id, body_shape);
        }
        break;
        case TileMapObjectType::Circle:
        {
            const TileMapCircle * circle = static_cast<const TileMapCircle *>(&__map_object);
            const float radius = graphicalToPhysical(circle->getRadius());
            if(radius <= .0f)
                break;
            b2Circle b2_circle
            {
                .center = { .x = .0f, .y = .0f },
                .radius = radius
            };
            b2ShapeId b2_shape_id = b2CreateCircleShape(b2_body_id, &b2_shape_def, &b2_circle);
            BodyShape * body_shape = &body->createShape(_body_options.shape_key.value_or(_class), circle->getId());
            b2Shape_SetUserData(b2_shape_id, body_shape);
        }
        break;
        default: return;
        }
    });
}

bool Scene::destroyBody(uint64_t _body_id)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
         return false;
    if(B2_ID_EQUALS(m_followed_body_id, b2_body_id))
        m_followed_body_id = b2_nullBodyId;
    if(int joints_count = b2Body_GetJointCount(b2_body_id))
    {
        std::vector<b2JointId> b2_joints(joints_count);
        b2Body_GetJoints(b2_body_id, b2_joints.data(), joints_count);
        for(const b2JointId & b2_joint_id : b2_joints)
            destroyJoint(getUserData(b2_joint_id)->getGid());
    }
    delete getUserData(b2_body_id);
    b2DestroyBody(b2_body_id);
    m_bodies.erase(_body_id);
    return true;
}

Body * Scene::getBody(uint64_t _body_id)
{
    auto it = m_bodies.find(_body_id);
    return it == m_bodies.end() ? nullptr : getUserData(it->second);
}

b2BodyId Scene::findBox2dBody(uint64_t _body_id) const
{
    auto it = m_bodies.find(_body_id);
    return it == m_bodies.end() ? b2_nullBodyId : it->second;
}

b2BodyType Scene::mapBodyType(BodyType _type)
{
    switch(_type)
    {
    case BodyType::Dynamic:
        return b2_dynamicBody;
    case BodyType::Kinematic:
        return b2_kinematicBody;
    default:
        return b2_staticBody;
    }
}

bool Scene::setFollowedBody(uint64_t _body_id)
{
    m_followed_body_id = findBox2dBody(_body_id);
    return B2_IS_NON_NULL(m_followed_body_id);
}

void Scene::resetFollowedBody()
{
    m_followed_body_id = b2_nullBodyId;
}

bool Scene::setBodyLayer(uint64_t _body_id, const std::string & _layer)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    getUserData(b2_body_id)->setLayer(_layer);
    return true;
}

GraphicsPack * Scene::getBodyShapeGraphicsPack(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphics_key)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return nullptr;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return nullptr;
    return shape->getGraphics(_graphics_key);
}

GraphicsPack * Scene::getBodyShapeCurrentGraphics(uint64_t _body_id, const PreHashedKey<std::string> & _shape_key)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return nullptr;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return nullptr;
    return shape->getCurrentGraphics();
}

bool Scene::setBodyShapeCurrentGraphics(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphic_key)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->setCurrentGraphics(_graphic_key);
}

bool Scene::flipBodyShapeGraphics(
    uint64_t _body_id,
    const PreHashedKey<std::string> & _shape_key,
    const PreHashedKey<std::string> & _graphic_key,
    bool _flip_horizontally,
    bool _flip_vertically)
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return false;
    BodyShape * shape = getUserData(b2_body_id)->findShape(_shape_key);
    if(shape == nullptr)
        return false;
    return shape->flipGraphics(_graphic_key, _flip_horizontally, _flip_vertically);
}

uint64_t Scene::createJoint(const DistanceJointDefenition & _definition)
{
    b2DistanceJointDef b2_joint_def = b2DefaultDistanceJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.enableSpring = _definition.is_spring_enabled;
    b2_joint_def.enableLimit = _definition.is_limit_enabled;
    b2_joint_def.enableMotor = _definition.is_motor_enabled;
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.local_anchor_a)
    {
        b2_joint_def.localAnchorA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_a->x),
            .y = graphicalToPhysical(_definition.local_anchor_a->y)
        };
    }
    if(_definition.local_anchor_b)
    {
        b2_joint_def.localAnchorB = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_b->x),
            .y = graphicalToPhysical(_definition.local_anchor_b->y)
        };
    }
    if(_definition.min_length)
        b2_joint_def.minLength = graphicalToPhysical(_definition.min_length.value());
    if(_definition.max_length)
        b2_joint_def.maxLength = graphicalToPhysical(_definition.max_length.value());
    if(_definition.hertz)
        b2_joint_def.hertz = _definition.hertz.value();
    if(_definition.damping_ratio)
        b2_joint_def.dampingRatio = _definition.damping_ratio.value();
    if(_definition.max_motor_force)
        b2_joint_def.maxMotorForce = _definition.max_motor_force.value();
    if(_definition.motor_speed)
        b2_joint_def.motorSpeed = _definition.motor_speed.value();
    if(_definition.length)
        b2_joint_def.length = graphicalToPhysical(_definition.length.value());
    b2JointId b2_joint_id = b2CreateDistanceJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new DistanceJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

uint64_t Scene::createJoint(const MotorJointDefinition & _definition)
{
    b2MotorJointDef b2_joint_def = b2DefaultMotorJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.linear_offset)
    {
        b2_joint_def.linearOffset = b2Vec2
        {
            .x = graphicalToPhysical(_definition.linear_offset->x),
            .y = graphicalToPhysical(_definition.linear_offset->y)
        };
    }
    if(_definition.angular_offset)
        b2_joint_def.angularOffset = _definition.angular_offset.value();
    if(_definition.max_force)
        b2_joint_def.maxForce = _definition.max_force.value();
    if(_definition.max_torque)
        b2_joint_def.maxTorque = _definition.max_torque.value();
    if(_definition.correction_factor)
        b2_joint_def.correctionFactor = _definition.correction_factor.value();
    b2JointId b2_joint_id = b2CreateMotorJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new MotorJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

uint64_t Scene::createJoint(const MouseJointDefinition & _definition)
{
    b2MouseJointDef b2_joint_def = b2DefaultMouseJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.target)
    {
        b2_joint_def.target = b2Vec2
        {
            .x = graphicalToPhysical(_definition.target->x),
            .y = graphicalToPhysical(_definition.target->y)
        };
    }
    if(_definition.hertz)
        b2_joint_def.hertz = _definition.hertz.value();
    if(_definition.max_force)
        b2_joint_def.maxForce = _definition.max_force.value();
    if(_definition.damping_ratio)
        b2_joint_def.dampingRatio = _definition.damping_ratio.value();
    b2JointId b2_joint_id = b2CreateMouseJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new MouseJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

uint64_t Scene::createJoint(const PrismaticJointDefinition & _definition)
{
    b2PrismaticJointDef b2_joint_def = b2DefaultPrismaticJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.enableSpring = _definition.is_spring_enabled;
    b2_joint_def.enableLimit = _definition.is_limit_enabled;
    b2_joint_def.enableMotor = _definition.is_motor_enabled;
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.local_anchor_a)
    {
        b2_joint_def.localAnchorA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_a->x),
            .y = graphicalToPhysical(_definition.local_anchor_a->y)
        };
    }
    if(_definition.local_anchor_b)
    {
        b2_joint_def.localAnchorB = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_b->x),
            .y = graphicalToPhysical(_definition.local_anchor_b->y)
        };
    }
    if(_definition.local_axis_a)
    {
        b2_joint_def.localAxisA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_axis_a->x),
            .y = graphicalToPhysical(_definition.local_axis_a->y)
        };
    }
    if(_definition.reference_angle)
        b2_joint_def.referenceAngle = _definition.reference_angle.value();
    if(_definition.hertz)
        b2_joint_def.hertz = _definition.hertz.value();
    if(_definition.damping_ratio)
        b2_joint_def.dampingRatio = _definition.damping_ratio.value();
    if(_definition.lower_translation)
        b2_joint_def.lowerTranslation = graphicalToPhysical(_definition.lower_translation.value());
    if(_definition.upper_translation)
        b2_joint_def.upperTranslation = graphicalToPhysical(_definition.upper_translation.value());
    if(_definition.max_motor_force)
        b2_joint_def.maxMotorForce = _definition.max_motor_force.value();
    if(_definition.motor_speed)
        b2_joint_def.motorSpeed = _definition.motor_speed.value();
    b2JointId b2_joint_id = b2CreatePrismaticJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new PrismaticJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

uint64_t Scene::createJoint(const WeldJointDefinition & _definition)
{
    b2WeldJointDef b2_joint_def = b2DefaultWeldJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.local_anchor_a)
    {
        b2_joint_def.localAnchorA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_a->x),
            .y = graphicalToPhysical(_definition.local_anchor_a->y)
        };
    }
    if(_definition.local_anchor_b)
    {
        b2_joint_def.localAnchorB = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_b->x),
            .y = graphicalToPhysical(_definition.local_anchor_b->y)
        };
    }
    if(_definition.reference_angle)
        b2_joint_def.referenceAngle = _definition.reference_angle.value();
    if(_definition.linear_hertz)
        b2_joint_def.linearHertz = _definition.linear_hertz.value();
    if(_definition.angular_hertz)
        b2_joint_def.angularHertz = _definition.angular_hertz.value();
    if(_definition.linear_damping_ratio)
        b2_joint_def.linearDampingRatio = _definition.linear_damping_ratio.value();
    if(_definition.angular_damping_ratio)
        b2_joint_def.angularDampingRatio = _definition.angular_damping_ratio.value();
    b2JointId b2_joint_id = b2CreateWeldJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new WeldJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

uint64_t Scene::createJoint(const WheelJointDefinition & _definition)
{
    b2WheelJointDef b2_joint_def = b2DefaultWheelJointDef();
    b2_joint_def.bodyIdA = findBox2dBody(_definition.body_a_id);
    b2_joint_def.bodyIdB = findBox2dBody(_definition.body_b_id);
    if(B2_IS_NULL(b2_joint_def.bodyIdA) | B2_IS_NULL(b2_joint_def.bodyIdB))
        return 0; // FIXEM: null?
    b2_joint_def.enableSpring = _definition.is_spring_enabled;
    b2_joint_def.enableLimit = _definition.is_limit_enabled;
    b2_joint_def.enableMotor = _definition.is_motor_enabled;
    b2_joint_def.collideConnected = _definition.is_collide_connected_enabled;
    if(_definition.local_anchor_a)
    {
        b2_joint_def.localAnchorA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_a->x),
            .y = graphicalToPhysical(_definition.local_anchor_a->y)
        };
    }
    if(_definition.local_anchor_b)
    {
        b2_joint_def.localAnchorB = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_anchor_b->x),
            .y = graphicalToPhysical(_definition.local_anchor_b->y)
        };
    }
    if(_definition.local_axis_a)
    {
        b2_joint_def.localAxisA = b2Vec2
        {
            .x = graphicalToPhysical(_definition.local_axis_a->x),
            .y = graphicalToPhysical(_definition.local_axis_a->y)
        };
    }
    if(_definition.lower_translation)
        b2_joint_def.lowerTranslation = graphicalToPhysical(_definition.lower_translation.value());
    if(_definition.upper_translation)
        b2_joint_def.upperTranslation = graphicalToPhysical(_definition.upper_translation.value());
    if(_definition.max_motor_torque)
        b2_joint_def.maxMotorTorque = _definition.max_motor_torque.value();
    b2JointId b2_joint_id = b2CreateWheelJoint(m_b2_world_id, &b2_joint_def);
    Joint * joint = new WheelJoint(b2_joint_id);
    b2Joint_SetUserData(b2_joint_id, joint);
    m_joints.insert(std::make_pair(joint->getGid(), b2_joint_id));
    return joint->getGid();
}

std::optional<DistanceJoint> Scene::getDistanceJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    DistanceJoint * joint = dynamic_cast<DistanceJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

std::optional<MotorJoint> Scene::getMotorJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    MotorJoint * joint = dynamic_cast<MotorJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

std::optional<MouseJoint> Scene::getMouseJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    MouseJoint * joint = dynamic_cast<MouseJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

std::optional<PrismaticJoint> Scene::getPrismaticJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    PrismaticJoint * joint = dynamic_cast<PrismaticJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

std::optional<WeldJoint> Scene::getWeldJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    WeldJoint * joint = dynamic_cast<WeldJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

std::optional<WheelJoint> Scene::getWheelJoint(uint64_t _id) const
{
    b2JointId b2_id = findJoint(_id);
    if(B2_IS_NULL(b2_id))
        return std::nullopt;
    WheelJoint * joint = dynamic_cast<WheelJoint *>(getUserData(b2_id));
    return joint ? std::make_optional(*joint) : std::nullopt;
}

bool Scene::destroyJoint(uint64_t _joint_id)
{
    b2JointId b2_joint_id = findJoint(_joint_id);
    if(B2_IS_NULL(b2_joint_id))
        return false;
    const Joint * joint = getUserData(b2_joint_id);
    m_joints.erase(joint->getGid());
    b2DestroyJoint(b2_joint_id);
    delete joint;
    return true;
}

b2JointId Scene::findJoint(uint64_t _joint_id) const
{
    auto it = m_joints.find(_joint_id);
    return it == m_joints.cend() ? b2_nullJointId : it->second;
}

bool Scene::loadTileMap(const std::filesystem::path & _file_path)
{
    deinitializeTileMap();
    m_tile_heap_ptr.reset();
    m_tile_map_ptr.reset();
    m_object_heap_ptr.reset();
    Tmx tmx = loadTmx(mr_renderer, mr_workspace, _file_path); // TODO: handle exceptions
    m_tile_heap_ptr = std::move(tmx.tile_heap);
    m_tile_map_ptr = std::move(tmx.tile_map);
    m_object_heap_ptr = std::move(tmx.object_heap);
    setClearColor(m_tile_map_ptr->getBackgroundColor());
    return m_tile_map_ptr != nullptr; // TODO: only exceptions
}

void Scene::step(const StepState & _state)
{
    if(!m_tile_map_ptr)
    {
        return;
    }
    m_defers.executeActions();
    b2World_Step(m_b2_world_id, _state.delta_time.count() / 1000.0f, 4); // TODO: stable rate (1.0f / 60.0f), all from user settings
    handleBox2dContactEvents();
    syncWorldWithFollowedBody();

    std::unordered_set<uint64_t> bodies_to_render(m_bodies.size());
    for(const auto & pair : m_bodies)
        bodies_to_render.insert(pair.first);
    drawLayersAndBodies(*m_tile_map_ptr, bodies_to_render, _state.delta_time);
    for(const uint64_t body_id : bodies_to_render)
        drawBody(m_bodies[body_id], _state.delta_time);

    if(mp_box2d_debug_draw)
        mp_box2d_debug_draw->draw();

    Observable<StepObserver>::callObservers(&StepObserver::onStepComplete, _state);
}

bool Scene::box2dPreSolveContact(b2ShapeId _shape_id_a, b2ShapeId _shape_id_b, b2Manifold * _manifold, void * _context)
{
    Scene * scene = static_cast<Scene *>(_context);
    bool result = true;
    PreSolveContact contact;
    if(!tryGetContactSide(_shape_id_a, contact.side_a) || !tryGetContactSide(_shape_id_b, contact.side_b))
        return true;
    contact.manifold = _manifold;
    scene->Observable<ContactObserver>::forEachObserver([&result, &contact](ContactObserver & __observer) {
        if(!__observer.preSolveContact(contact))
            result = false;
        return result;
    });
    return result;
}

void Scene::handleBox2dContactEvents()
{
    {
        Contact contact;
        b2ContactEvents contact_events = b2World_GetContactEvents(m_b2_world_id);
        for(int i = 0; i < contact_events.beginCount; ++i)
        {
            const b2ContactBeginTouchEvent & event = contact_events.beginEvents[i];
            if(tryGetContactSide(event.shapeIdA, contact.side_a) && tryGetContactSide(event.shapeIdB, contact.side_b))
                Observable<ContactObserver>::callObservers(&ContactObserver::beginContact, contact);
        }
        for(int i = 0; i < contact_events.endCount; ++i)
        {
            const b2ContactEndTouchEvent & event = contact_events.endEvents[i];
            if(tryGetContactSide(event.shapeIdA, contact.side_a) && tryGetContactSide(event.shapeIdB, contact.side_b))
                Observable<ContactObserver>::callObservers(&ContactObserver::endContact, contact);
        }
    }

    {
        SensorContact contact;
        b2SensorEvents sensor_events = b2World_GetSensorEvents(m_b2_world_id);
        for(int i = 0; i < sensor_events.beginCount; ++i)
        {
            const b2SensorBeginTouchEvent & event = sensor_events.beginEvents[i];
            if(tryGetContactSide(event.sensorShapeId, contact.sensor) &&
                tryGetContactSide(event.visitorShapeId, contact.visitor))
            {
                Observable<ContactObserver>::callObservers(&ContactObserver::beginSensorContact, contact);
            }
        }
        for(int i = 0; i < sensor_events.endCount; ++i)
        {
            const b2SensorEndTouchEvent & event = sensor_events.endEvents[i];
            if(tryGetContactSide(event.sensorShapeId, contact.sensor) &&
                tryGetContactSide(event.visitorShapeId, contact.visitor))
            {
                Observable<ContactObserver>::callObservers(&ContactObserver::endSensorContact, contact);
            }
        }
    }
}

bool Scene::tryGetContactSide(b2ShapeId _shape_id, ContactSide & _contact_side)
{
    b2BodyId b2_body_id = b2Shape_GetBody(_shape_id);
    const BodyShape * shape = getUserData(_shape_id);
    const Body * body = getUserData(b2_body_id);
    if(shape && body)
    {
        _contact_side.body_id = body->getGid();
        _contact_side.shape_key = shape->getKey();
        _contact_side.tile_map_object_id = shape->getTileMapObjectId();
        return true;
    }
    return false;
}

void Scene::syncWorldWithFollowedBody()
{
    if(B2_IS_NULL(m_followed_body_id))
    {
        return;
    }
    const FSize output_size = mr_renderer.getOutputSize();
    b2Vec2 followed_body_position = b2Body_GetPosition(m_followed_body_id);
    m_world_offset.x = physicalToGraphical(followed_body_position.x) - output_size.w / 2;
    m_world_offset.y = physicalToGraphical(followed_body_position.y) - output_size.h / 2;
    const int32_t map_x = m_tile_map_ptr->getX() * m_tile_map_ptr->getTileWidth();
    const int32_t map_y = m_tile_map_ptr->getY() * m_tile_map_ptr->getTileHeight();
    if(m_world_offset.x < map_x)
    {
        m_world_offset.x = map_x;
    }
    else
    {
        const float max_offset_x = m_tile_map_ptr->getWidth() * m_tile_map_ptr->getTileWidth() - output_size.w;
        if(m_world_offset.x > max_offset_x)
            m_world_offset.x = max_offset_x;
    }
    if(m_world_offset.y < map_y)
    {
        m_world_offset.y = map_y;
    }
    else
    {
        const float max_offset_y = m_tile_map_ptr->getHeight() * m_tile_map_ptr->getTileHeight() - output_size.h;
        if(m_world_offset.y > max_offset_y)
            m_world_offset.y = max_offset_y;
    }
}

void Scene::drawBody(b2BodyId _body_id, std::chrono::milliseconds _delta_time)
{
    SDL_FPoint body_position;
    {
        b2Vec2 pos = b2Body_GetPosition(_body_id);
        body_position = toAbsoluteCoords(physicalToGraphical(pos.x), physicalToGraphical(pos.y));
    }
    int shape_count = b2Body_GetShapeCount(_body_id);
    std::vector<b2ShapeId> shapes(shape_count);
    b2Body_GetShapes(_body_id, shapes.data(), shape_count);
    b2Rot b2_rotation = b2Body_GetRotation(_body_id);
    Rotation rotation(b2_rotation.s, b2_rotation.c); // TODO: https://github.com/libsdl-org/SDL/issues/11279
    for(const b2ShapeId & shape_id : shapes)
    {
        BodyShape * shape = getUserData(shape_id);
        GraphicsPack * graphics = shape->getCurrentGraphics();
        if(graphics)
        {
            // TODO: How to rotate multiple shapes?
            graphics->render(body_position, rotation, _delta_time);
        }
    }
    // if(mp_box2d_debug_draw)
    // {
    //     SDL_SetRenderDrawColor(&mr_renderer, 0, 128, 255, 255);
    //     SDL::sdlRenderCircle(&mr_renderer, body_position, 8.0f);
    //     SDL_RenderLine(&mr_renderer, body_position.x - 8, body_position.y, body_position.x + 8, body_position.y);
    //     SDL_RenderLine(&mr_renderer, body_position.x, body_position.y - 8, body_position.x, body_position.y + 8);
    // }
}

void Scene::drawLayersAndBodies(
    const TileMapLayerContainer & _container,
    std::unordered_set<uint64_t> & _bodies_to_render,
    std::chrono::milliseconds _delta_time)
{
    _container.forEachLayer([&_bodies_to_render, _delta_time, this](const TileMapLayer & __layer) {
        if(!__layer.isVisible()) return;
        switch(__layer.getType())
        {
        case TileMapLayerType::Tile:
            drawTileLayer(dynamic_cast<const TileMapTileLayer &>(__layer));
            break;
        case TileMapLayerType::Object:
            if(mr_workspace.isDebugRenderingEnabled())
                drawObjectLayer(dynamic_cast<const TileMapObjectLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::Image:
            drawImageLayer(dynamic_cast<const TileMapImageLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::Group:
        {
            const TileMapGroupLayer & group = dynamic_cast<const TileMapGroupLayer &>(__layer);
            if(group.isVisible())
                drawLayersAndBodies(group, _bodies_to_render, _delta_time);
            break;
        }}
        for(const auto & pair : m_bodies)
        {
            b2BodyId box2d_body_id = pair.second;
            const Body * body = getUserData(box2d_body_id);
            if(body->getLayer() == __layer.getName() && _bodies_to_render.erase(pair.first))
                drawBody(box2d_body_id, _delta_time);
        }
    });
}

void Scene::drawObjectLayer(const TileMapObjectLayer & _layer)
{
    // TODO: offset and parallax


    _layer.forEachObject([this](const TileMapObject & __object) {
        if(!__object.isVisible()) return;
        // TODO: if in viewport
        switch(__object.getObjectType())
        {
        case TileMapObjectType::Polygon:
            drawPolyXObject(dynamic_cast<const TileMapPolygon &>(__object), true);
            break;
        case TileMapObjectType::Polyline:
            drawPolyXObject(dynamic_cast<const TileMapPolyline &>(__object), false);
            break;
        case TileMapObjectType::Circle:
            drawCircle(dynamic_cast<const TileMapCircle &>(__object));
            break;
        default:
            // TODO: point
            break;
        }
    });
}

void Scene::drawPolyXObject(const TileMapPolyX & _poly, bool _close)
{
    const std::vector<SDL_FPoint> & poly_points = _poly.getPoints();
    size_t poly_points_count = poly_points.size();
    if(poly_points_count < 2) return;
    SDL_FPoint base_point = toAbsoluteCoords(_poly.getPosition().x, _poly.getPosition().y);
    std::vector<SDL_FPoint> points(poly_points.size());
    for(size_t i = 0; i < poly_points_count; ++i)
    {
        points[i].x = base_point.x + poly_points[i].x;
        points[i].y = base_point.y + poly_points[i].y;
    }
    mr_renderer.renderPolyline(points, gc_object_debug_color, _close);
}

void Scene::drawCircle(const TileMapCircle & _circle)
{
    mr_renderer.renderCircle(CircleRenderingData(
        toAbsoluteCoords(_circle.getPosition().x, _circle.getPosition().y),
        _circle.getRadius(),
        gc_object_debug_color));
}

void Scene::drawTileLayer(const TileMapTileLayer & _layer)
{
    const SDL_FRect viewport = calculateViewport(_layer);
    const float first_col = std::floor(viewport.x / m_tile_map_ptr->getTileWidth());
    const float first_row = std::floor(viewport.y / m_tile_map_ptr->getTileHeight());
    const float last_col = std::ceil((viewport.x + viewport.w) / m_tile_map_ptr->getTileWidth());
    const float last_row = std::ceil((viewport.y + viewport.h) / m_tile_map_ptr->getTileHeight());
    const SDL_FPoint start_position =
    {
        .x = first_col * m_tile_map_ptr->getTileWidth() - viewport.x,
        .y = first_row * m_tile_map_ptr->getTileHeight() - viewport.y
    };
    const FSize tile_map_cell_size(
        static_cast<float>(m_tile_map_ptr->getTileWidth()),
        static_cast<float>(m_tile_map_ptr->getTileHeight())
    );

    std::unordered_set<const TileMapTileLayerCell *> extra_cells;
    SDL_FRect tile_rect;
    SDL_FRect dest_rect;

    for(
        int32_t row = static_cast<int32_t>(first_row), dest_row = 0;
        row <= static_cast<int32_t>(last_row);
        ++row, ++dest_row
    ) {
        for(
            int32_t col = static_cast<int32_t>(first_col), dest_col = 0;
            col <= static_cast<int32_t>(last_col);
            ++col, ++dest_col
        ) {
            const TileMapTileLayerCell * cell = _layer.getCell(col, row);
            if(!cell) continue;

            if(!cell->master_cells.empty())
            {
                // The large tiles are partially visible, but their main tile is outside the viewport
                extra_cells.insert(cell->master_cells.cbegin(), cell->master_cells.cend());
            }

            if(!cell->tile) continue;

            tile_rect.x = cell->tile->getSourceX();
            tile_rect.y = cell->tile->getSourceY();
            tile_rect.w = cell->tile->getWidth();
            tile_rect.h = cell->tile->getHeight();

            dest_rect.x = start_position.x + dest_col * tile_map_cell_size.w;
            dest_rect.y = start_position.y + dest_row * tile_map_cell_size.h;
            dest_rect.w = tile_rect.w;
            dest_rect.h = tile_rect.h;

            if(tile_rect.h < tile_map_cell_size.h)
                dest_rect.y += tile_map_cell_size.h - tile_rect.h;
            else if(tile_rect.h > tile_map_cell_size.h)
                dest_rect.y -= tile_rect.h - tile_map_cell_size.h;

            mr_renderer.renderTexture(TextureRenderingData(
                dest_rect,
                cell->tile->getSource(),
                tile_rect));
        }
    }

    for(const auto * cell : extra_cells)
    {
        if(cell->x >= first_col && cell->x < last_col && cell->y >= first_row && cell->y < last_row)
        {
            // Already drawn by loop above
            continue;
        }
        tile_rect.x = cell->tile->getSourceX();
        tile_rect.y = cell->tile->getSourceY();
        tile_rect.w = cell->tile->getWidth();
        tile_rect.h = cell->tile->getHeight();

        dest_rect.x = (_layer.getX() + cell->x) * tile_map_cell_size.w - viewport.x;
        dest_rect.y = (_layer.getY() + cell->y) * tile_map_cell_size.h - viewport.y;
        dest_rect.w = tile_rect.w;
        dest_rect.h = tile_rect.h;

        if(tile_rect.h > tile_map_cell_size.h)
            dest_rect.y -= tile_rect.h - tile_map_cell_size.h;

        mr_renderer.renderTexture(TextureRenderingData(
            dest_rect,
            cell->tile->getSource(),
            tile_rect));
    }
}

SDL_FRect Scene::calculateViewport(const TileMapTileLayer & _layer) const
{
    SDL_FRect viewport;
    SDL_FPoint layer_offset { .x = _layer.getOffsetX(), .y = _layer.getOffsetY() };
    SDL_FPoint layer_parallax { .x = _layer.getParallaxX(), .y = _layer.getParallaxY() };
    for(const auto * parent_layer = _layer.getParent(); parent_layer; parent_layer = parent_layer->getParent())
    {
        layer_offset.x += parent_layer->getOffsetX();
        layer_offset.y += parent_layer->getOffsetY();
        layer_parallax.x *= parent_layer->getParallaxX();
        layer_parallax.y *= parent_layer->getParallaxY();
    }
    viewport.x = m_world_offset.x * layer_parallax.x - layer_offset.x;
    viewport.y = m_world_offset.y * layer_parallax.y - layer_offset.y;
    const FSize & output_size = mr_renderer.getOutputSize();
    viewport.w = output_size.w;
    viewport.h = output_size.h;
    return viewport;
}

void Scene::drawImageLayer(const TileMapImageLayer & _layer)
{
    // TODO: offset and parallax

    const Texture & image = _layer.getImage();
    SDL_FRect dim { .0f, .0f, image.getWidth(), image.getHeight() };
    mr_renderer.renderTexture(TextureRenderingData(dim, image, std::nullopt));
}

bool Scene::doesBodyExist(uint64_t _body_id) const
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    return B2_IS_NON_NULL(b2_body_id);
}

bool Scene::doesBodyShapeExist(uint64_t _body_id, const PreHashedKey<std::string> & _shape_key) const
{
    b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NON_NULL(b2_body_id))
    {
        Body * body = getUserData(b2_body_id);
        return body->findShape(_shape_key) != nullptr;
    }
    return false;
}

std::optional<std::vector<SDL_FPoint>> Scene::findPath(
    uint64_t _body_id,
    const SDL_FPoint & _destination,
    bool _allow_diagonal_steps,
    bool _avoid_sensors) const
{
    const b2BodyId b2_body_id = findBox2dBody(_body_id);
    if(B2_IS_NULL(b2_body_id))
        return std::nullopt;
    AStarOptions options;
    options.allow_diagonal_steps = _allow_diagonal_steps;
    options.avoid_sensors = _avoid_sensors;
    auto b2_result = aStarFindPath(m_b2_world_id, b2_body_id, toBox2D(_destination), options);
    if(!b2_result.has_value())
        return std::nullopt;
    std::vector<SDL_FPoint> result;
    result.reserve(b2_result.value().size());
    for(size_t i = 0; i < b2_result.value().size(); ++i)
        result.push_back(toSDL(b2_result.value()[i]));
    return result;
}
