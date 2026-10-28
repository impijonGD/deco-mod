#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <random>
#include <cmath>

using namespace geode::prelude;

static float rnd(float range) {
	static std::mt19937 gen{std::random_device{}()};
	if (range <= 0.f) return 0.f;
	return std::uniform_real_distribution<float>(-range, range)(gen);
}

class $modify(DecoEditorUI, EditorUI) {
	bool init(LevelEditorLayer* lel) {
		if (!EditorUI::init(lel)) return false;

		auto winSize = CCDirector::get()->getWinSize();

		auto menu = CCMenu::create();
		menu->setID("deco-plus-menu"_spr);
		menu->setLayout(
			ColumnLayout::create()
				->setAxisReverse(true)
				->setGap(4.f)
				->setAutoScale(false)
		);
		menu->setContentSize({60.f, 140.f});

		auto addBtn = [&](char const* text, SEL_MenuHandler handler) {
			auto spr = ButtonSprite::create(text, 50, true, "bigFont.fnt", "GJ_button_04.png", 28.f, 0.5f);
			menu->addChild(CCMenuItemSpriteExtra::create(spr, this, handler));
		};
		addBtn("Rand", menu_selector(DecoEditorUI::onDecoRandom));
		addBtn("Circle", menu_selector(DecoEditorUI::onDecoCircle));
		addBtn("Line", menu_selector(DecoEditorUI::onDecoLine));
		menu->updateLayout();

		menu->setPosition({winSize.width - 40.f, winSize.height / 2.f + 20.f});
		this->addChild(menu, 100);
		return true;
	}

	void onDecoRandom(CCObject*) {
		auto objs = this->getSelectedObjects();
		if (!objs || objs->count() == 0) {
			return FLAlertLayer::create("Deco Plus", "Zaznacz najpierw jakieś obiekty.", "OK")->show();
		}
		auto rot = Mod::get()->getSettingValue<double>("random-rotation");
		auto scl = Mod::get()->getSettingValue<double>("random-scale");
		auto off = Mod::get()->getSettingValue<double>("random-offset");

		for (auto obj : CCArrayExt<GameObject*>(objs)) {
			obj->setRotation(obj->getRotation() + rnd(rot));
			float f = 1.f + rnd(scl);
			obj->updateCustomScaleX(obj->m_scaleX * f);
			obj->updateCustomScaleY(obj->m_scaleY * f);
			this->moveObject(obj, {rnd(off), rnd(off)});
		}
	}

	void onDecoCircle(CCObject*) {
		auto objs = this->getSelectedObjects();
		if (!objs || objs->count() < 2) {
			return FLAlertLayer::create("Deco Plus", "Zaznacz co najmniej 2 obiekty.", "OK")->show();
		}
		float radius = static_cast<float>(Mod::get()->getSettingValue<double>("circle-radius"));

		CCPoint center = {0.f, 0.f};
		for (auto obj : CCArrayExt<GameObject*>(objs)) center += obj->getPosition();
		center = center / static_cast<float>(objs->count());

		int n = objs->count();
		int i = 0;
		for (auto obj : CCArrayExt<GameObject*>(objs)) {
			float a = 2.f * static_cast<float>(M_PI) * i / n;
			CCPoint target = center + CCPoint{std::cos(a) * radius, std::sin(a) * radius};
			this->moveObject(obj, target - obj->getPosition());
			i++;
		}
	}

	void onDecoLine(CCObject*) {
		auto objs = this->getSelectedObjects();
		if (!objs || objs->count() < 3) {
			return FLAlertLayer::create("Deco Plus", "Zaznacz co najmniej 3 obiekty.", "OK")->show();
		}
		int n = objs->count();
		auto first = static_cast<GameObject*>(objs->objectAtIndex(0));
		auto last = static_cast<GameObject*>(objs->objectAtIndex(n - 1));
		CCPoint p0 = first->getPosition();
		CCPoint p1 = last->getPosition();

		for (int i = 1; i < n - 1; i++) {
			auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
			float t = static_cast<float>(i) / (n - 1);
			CCPoint target = p0 + (p1 - p0) * t;
			this->moveObject(obj, target - obj->getPosition());
		}
	}
};
