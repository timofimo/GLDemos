#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <algorithm>

class Transform
{
	enum EFlags
	{
		CHANGED = 0x01,
		PARENT_CHANGED = 0x02,
		ORIENTATION_CHANGED = 0x04,
		ORIENTATION_CHANGED_Q = 0x08,
	};

public:
	Transform() : m_localPosition(), m_localOrientation(), m_localOrientationQ(), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF){}
	Transform(const glm::vec3& position) : m_localPosition(position), m_localOrientation(), m_localOrientationQ(), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(const glm::vec3& position, const glm::vec3& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localOrientationQ(), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(), m_localOrientationQ(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(Transform* parent) : m_localPosition(), m_localOrientation(), m_localOrientationQ(), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position) : m_localPosition(position), m_localOrientation(), m_localOrientationQ(), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position, const glm::vec3& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localOrientationQ(), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(), m_localOrientationQ(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	~Transform()
	{
		RemoveFromParent(m_parent);
		for (Transform* child : m_children)
			child->SetParent(nullptr);
	}

	void SetParent(Transform* parent)
	{
		if (m_parent)
			RemoveFromParent(m_parent);

		m_parent = parent;

		if(parent)
			AnnounceToParent(parent);
	}
	void Translate(const glm::vec3& translation, bool worldSpace = true)
	{
		if (worldSpace)
		{
			m_localPosition += GetRight() * translation.x + GetUp() * translation.y + GetForward() * translation.z;
		}
		else
			m_localPosition += translation;

		SetChanged();
	}
	void Rotate(const glm::vec3& rotation)
	{
		m_localOrientation += rotation;
		SetChanged();
		m_flags |= ORIENTATION_CHANGED;
	}
	void Rotate(const glm::quat& rotation)
	{
		m_localOrientationQ *= rotation;
		SetChanged();
		m_flags |= ORIENTATION_CHANGED_Q;
	}
	void Scale(const glm::vec3& scaling)
	{
		m_localScale *= scaling;
		SetChanged();
	}

	void SetPosition(const glm::vec3& position)
	{
		m_localPosition = position;
		SetChanged();
	}
	void SetOrientation(const glm::vec3& orientation)
	{
		m_localOrientation = orientation;
		SetChanged();
		m_flags |= ORIENTATION_CHANGED;
	}
	void SetOrientation(const glm::quat& orientation)
	{
		m_localOrientationQ = orientation;
		SetChanged();
		m_flags |= ORIENTATION_CHANGED_Q;
	}
	void SetScale(const glm::vec3& scale)
	{
		m_localScale = scale;
		SetChanged();
	}

	const glm::vec3& GetPosition() const
	{
		return m_localPosition;
	}
	const glm::vec3& GetOrientation()
	{
		if(m_flags & ORIENTATION_CHANGED_Q)
		{
			m_flags &= ~ORIENTATION_CHANGED_Q;
			m_localOrientation = glm::eulerAngles(m_localOrientationQ);
		}
		return m_localOrientation;
	}
	const glm::quat& GetOrientationQ()
	{
		if(m_flags & ORIENTATION_CHANGED)
		{
			m_flags &= ~ORIENTATION_CHANGED;
			m_localOrientationQ = glm::quat(m_localOrientation);
		}
		return m_localOrientationQ;
	}
	const glm::vec3& GetScale() const
	{
		return m_localScale;
	}
	const glm::mat4& GetMatrix()
	{
		if(m_flags & CHANGED)
		{
			m_flags &= ~CHANGED;
			m_localMatrix = glm::translate(GetPosition()) * glm::toMat4(GetOrientationQ()) * glm::scale(GetScale());
		}
		return m_localMatrix;
	}
	glm::vec3 GetWorldPosition()
	{
		return GetWorldMatrix() * glm::vec4(m_localPosition.x, m_localPosition.y, m_localPosition.z, 1.0f);
	}
	glm::vec3 GetWorldOrientation()
	{
		if (m_parent)
			return m_parent->GetWorldOrientation() + GetOrientation();

		return GetOrientation();
	}
	glm::quat GetWorldOrientationQ()
	{
		return glm::quat(GetWorldMatrix());
	}
	glm::vec3 GetWorldScale() const
	{
		if (m_parent)
			return m_parent->GetWorldScale() * GetScale();

		return GetScale();
	}
	const glm::mat4& GetWorldMatrix()
	{
		if (((m_flags & CHANGED) || (m_flags & PARENT_CHANGED)) && m_parent)
		{
			m_flags &= ~(CHANGED | PARENT_CHANGED);
			m_worldMatrix = m_parent->GetWorldMatrix() * GetMatrix();
		}
		
		return m_worldMatrix;
	}
	glm::vec3 GetForward()
	{
		return glm::mat3(GetWorldMatrix()) * glm::vec3(0.0f, 0.0f, 1.0f);
	}
	glm::vec3 GetRight()
	{
		return glm::mat3(GetWorldMatrix()) * glm::vec3(1.0f, 0.0f, 0.0f);
	}
	glm::vec3 GetUp()
	{
		return glm::mat3(GetWorldMatrix()) * glm::vec3(0.0f, 1.0f, 0.0f);
	}
	glm::mat4 GetViewMatrix()
	{
		glm::vec3 pos = GetWorldPosition();
		return glm::lookAt(pos, pos + GetForward(), GetUp());
	}

private:
	void SetChanged()
	{
		if (!(m_flags & CHANGED))
			NotifyChildrenOfChange();
		m_flags |= CHANGED;
	}
	void AnnounceToParent(Transform* parent)
	{
		parent->m_children.push_back(this);
	}
	void RemoveFromParent(Transform* parent)
	{
		parent->m_children.erase(std::remove_if(parent->m_children.begin(), parent->m_children.end(), this));
	}
	void NotifyChildrenOfChange()
	{
		for (Transform* child : m_children)
		{
			child->m_flags |= PARENT_CHANGED;
			child->NotifyChildrenOfChange();
		}
	}

	glm::vec3 m_localPosition;
	glm::vec3 m_localOrientation;
	glm::quat m_localOrientationQ;
	glm::vec3 m_localScale;
	glm::mat4 m_localMatrix;
	glm::mat4 m_worldMatrix;

	Transform* m_parent;
	std::vector<Transform*> m_children;

	char m_flags;
};
