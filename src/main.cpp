#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCSprite.hpp>

using namespace geode::prelude;

std::vector<GameObject*> checkpoints;
std::unordered_map<CCObject*, GLubyte> opacities;

class $modify(PlayLayer) {
	void updateCheckpoints() {
		if(Mod::get()->getSettingValue<bool>("platformer-only") && !this->m_level->isPlatformer()) return;
		opacities.clear();
		int pos = checkpoints.size();
		for(auto checkpoint : checkpoints) {
			pos--;
			opacities[checkpoint] = std::ceil(Mod::get()->getSettingValue<int64_t>("default") * 2.55 * std::pow(Mod::get()->getSettingValue<double>("multiplier"), pos));
			checkpoint->setOpacity(opacities[checkpoint]);
		}
	}

	$override
	void storeCheckpoint(CheckpointObject* p0) {
		PlayLayer::storeCheckpoint(p0);
		checkpoints.push_back(p0->m_physicalCheckpointObject);
		updateCheckpoints();
	}

	$override
	void removeCheckpoint(bool p0) {
		PlayLayer::removeCheckpoint(p0);
		if(!checkpoints.empty()) checkpoints.pop_back();
		updateCheckpoints();
	}

	$override
	void removeAllCheckpoints() {
		PlayLayer::removeAllCheckpoints();
		checkpoints.clear();
		opacities.clear();
	}

	$override
	void onExit() {
		PlayLayer::onExit();
		checkpoints.clear();
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