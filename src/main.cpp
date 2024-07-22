#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCSprite.hpp>

using namespace geode::prelude;

std::unordered_map<CCObject*, GLubyte> opacities;

class $modify(PlayLayer) {
	void updateCheckpoints() {
		if(Mod::get()->getSettingValue<bool>("platformer-only") && !this->m_level->isPlatformer()) return;
		opacities.clear();
		int pos = m_checkpointArray->count();
		CCObject* child;
		CCARRAY_FOREACH(m_checkpointArray, child) {
			auto checkpoint = static_cast<CheckpointObject*>(child)->getObject();
			pos--;
			opacities[checkpoint] = std::ceil(Mod::get()->getSettingValue<int64_t>("default") * 2.55 * std::pow(Mod::get()->getSettingValue<double>("multiplier"), pos));
			checkpoint->setOpacity(opacities[checkpoint]);
		}
	}

	$override
	void storeCheckpoint(CheckpointObject* p0) {
		PlayLayer::storeCheckpoint(p0);
		updateCheckpoints();
	}

	$override
	void removeCheckpoint(bool p0) {
		PlayLayer::removeCheckpoint(p0);
		updateCheckpoints();
	}

	$override
	void removeAllCheckpoints() {
		PlayLayer::removeAllCheckpoints();
		opacities.clear();
	}

	$override
	void startGame() {
		PlayLayer::startGame();
		opacities.clear();
	}
};

class $modify(CCSprite) {
	$override
	void setOpacity(GLubyte opacity) {
		if(opacities.contains(this)) opacity = opacity == 0 ? opacities[this] : opacity * opacities[this] / 255;
		CCSprite::setOpacity(opacity);
	}
};