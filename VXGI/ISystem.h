#pragma once

class ExampleBase;

class ISystem
{
public:
	virtual ~ISystem() = default;
	ISystem(const std::string& name) : m_name(name){}

	void OpenSettingsWindow() { m_windowOpen = true; }
	virtual void DrawSettingsWindow() = 0;

	virtual void Update(float deltatime, ExampleBase* base) = 0;
	virtual void Render(ExampleBase* base) = 0;

	const std::string& GetName() const { return m_name; }

protected:
	bool m_windowOpen = false;

private:
	std::string m_name;
};
