#include "ClipboardExplorer.hpp"
#include "../MainWindow.hpp"
using namespace std::placeholders;

std::pair<TBStyle, TBStyle> ClipboardExplorer::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
}

void ClipboardExplorer::MarkFn(std::size_t, MarkType) { }

ClipboardExplorer::ClipboardExplorer(MainWindow* main):
	ClipboardExplorerBase(std::bind(&ClipboardExplorer::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&ClipboardExplorer::MarkFn, this, _1, _2)),
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

	AddKeyboardInput(Settings::Keys::Clipboard::exit, [this]
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
		SetEntries(gClipboard.size());
	}, EventWhen::BEFORE);

	SetBackground(Settings::Style::Position::background);
}

ClipboardExplorer::~ClipboardExplorer()
{
	
}
