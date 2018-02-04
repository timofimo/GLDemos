#pragma once

class Transform
{
	enum EFlags
	{
		CHANGED = 0x01,
		PARENT_CHANGED = 0x02,
	};

public:
	Transform() : m_localPosition(), m_localOrientation(1, 0, 0, 0), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF){}
	Transform(const glm::vec3& position) : m_localPosition(position), m_localOrientation(1, 0, 0, 0), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(const glm::vec3& position, const glm::vec3& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) {}
	Transform(Transform* parent) : m_localPosition(), m_localOrientation(1, 0, 0, 0), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position) : m_localPosition(position), m_localOrientation(1, 0, 0, 0), m_localScale(1.0f), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position, const glm::vec3& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
	Transform(Transform* parent, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale) : m_localPosition(position), m_localOrientation(orientation), m_localScale(scale), m_localMatrix(), m_worldMatrix(), m_parent(nullptr), m_children(), m_flags(0xFF) { SetParent(parent); }
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
	void Translate(const glm::vec3& translation, bool worldSpace = false)
	{
		if (worldSpace)
			m_localPosition += glm::inverse(GetWorldOrientation()) * translation;
		else
			m_localPosition += translation;

		SetChanged();
	}
	void Rotate(const glm::vec3& rotation)
	{
		m_localOrientation = glm::quat(glm::vec3(0.0f, rotation.y, 0.0f)) * m_localOrientation * glm::quat(glm::vec3(rotation.x, 0.0f, 0.0f));
		SetChanged();
	}
	void Rotate(const glm::quat& rotation)
	{
		m_localOrientation = rotation * m_localOrientation;
		SetChanged();
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
		m_localOrientation = glm::quat(orientation);
		SetChanged();
	}
	void SetOrientation(const glm::quat& orientation)
	{
		m_localOrientation = orientation;
		SetChanged();
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
	glm::vec3 GetOrientationEuler() const
	{
		return glm::eulerAngles(m_localOrientation);
	}
	const glm::quat& GetOrientation() const
	{
		return m_localOrientation;
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
			m_localMatrix = glm::translate(GetPosition()) * glm::mat4_cast(GetOrientation()) * glm::scale(GetScale());
		}
		return m_localMatrix;
	}
	glm::vec3 GetWorldPosition()
	{
		return glm::vec3(GetWorldMatrix()[3]);
	}
	glm::quat GetWorldOrientation() const
	{
		if (m_parent)
			return m_parent->GetWorldOrientation() * GetOrientation();

		return GetOrientation();
	}
	glm::vec3 GetWorldScale() const
	{
		if (m_parent)
			return m_parent->GetWorldScale() * GetScale();

		return GetScale();
	}
	const glm::mat4& GetWorldMatrix()
	{
		if (m_parent)
		{
			if ((m_flags & CHANGED) || (m_flags & PARENT_CHANGED))
			{
				m_flags &= ~(CHANGED | PARENT_CHANGED);
				m_worldMatrix = m_parent->GetWorldMatrix() * GetMatrix();
			}
		}
		else if (m_flags & CHANGED)
			m_worldMatrix = GetMatrix();
		
		return m_worldMatrix;
	}
	glm::vec3 GetForward() const
	{
		return glm::normalize(GetWorldOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));
	}
	glm::vec3 GetRight() const
	{
		return glm::normalize(GetWorldOrientation() * glm::vec3(-1.0f, 0.0f, 0.0f));
	}
	glm::vec3 GetUp() const
	{
		return glm::normalize(GetWorldOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
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
		if(parent)
			parent->m_children.erase(std::remove_if(parent->m_children.begin(), parent->m_children.end(), [this](const Transform* other) {return other == this; }));
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
	glm::quat m_localOrientation;
	glm::vec3 m_localScale;
	glm::mat4 m_localMatrix;
	glm::mat4 m_worldMatrix;

	Transform* m_parent;
	std::vector<Transform*> m_children;

	unsigned char m_flags;
};
