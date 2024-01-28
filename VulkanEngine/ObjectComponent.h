#pragma once

class ObjectComponent
{
public:
    virtual ~ObjectComponent();
    
    template<class T = ObjectComponent, std::enable_if_t<std::is_base_of_v<ObjectComponent, T>>* = nullptr>
    static T* CreateComponent(std::string _name = "Component");

    virtual void AttachTo(ObjectComponent* _parentComp);
    void RemoveFrom(ObjectComponent* _parentComp) const;
    
    virtual void TickComponent(float _deltaSeconds);
    
    /* Getters */
    std::string GetName() const { return m_name; }
    std::vector<ObjectComponent*> GetAttachedComponents() const { return m_attachedComponents; }

    /* Setters */
    void SetName(const std::string& _name) { m_name = _name; }
    
protected:
    std::string m_name;
    std::vector<ObjectComponent*> m_attachedComponents;
};

template<class T = ObjectComponent, std::enable_if_t<std::is_base_of_v<ObjectComponent, T>>*>
T* ObjectComponent::CreateComponent(std::string _name)
{
    T* createdComp = new T();

    createdComp->m_name = _name;
    
    return createdComp;
}
