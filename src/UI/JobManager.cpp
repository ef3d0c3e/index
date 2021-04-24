#include "JobManager.hpp"
#include "../MainWindow.hpp"
#include <fmt/chrono.h>
using namespace std::placeholders;

std::pair<TBStyle, TBStyle> JobManager::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
}

void JobManager::MarkFn(std::size_t, MarkType) { }

JobManager::JobManager(MainWindow* main):
	JobManagerBase(std::bind(&JobManager::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&JobManager::MarkFn, this, _1, _2)),
	m_main(main)
{
	// Input
	AddKeyboardInput(Settings::Keys::List::down, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionDown();
		else [[unlikely]]
			ActionDownN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::down_page, [this]
	{
		ActionDownN(10);
	});

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_DOWN,
				[this](const Vec2i&){ ActionDownN(1); }));

	AddKeyboardInput(Settings::Keys::List::up, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionUp();
		else [[unlikely]]
			ActionUpN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::up_page, [this]
	{
		ActionUpN(10);
	});

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(Settings::Keys::Go::top,  [this]
	{
		ActionSetPosition(0);
	});

	AddKeyboardInput(Settings::Keys::Go::bottom, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	});

	AddKeyboardInput(Settings::Keys::JobManager::exit, [this]
	{
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	OnChangePosition.AddEvent([this]()
	{
		// Updating the repeat
		m_main->SetWidgetExpired(m_main->GetTablineID(), true);
	});

	// Update size before redraws
	OnDraw.AddEvent([this]
	{
		SetEntries(gJobList.size());
	}, EventWhen::BEFORE);

	SetBackground(Settings::Style::Position::background);
}

JobManager::~JobManager()
{
	
}
