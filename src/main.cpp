#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayer) {
	struct Fields {
		bool m_disabled;
		std::unordered_map<CheckpointObject*, int> m_opacities;
		CCNode* m_checkpointNode;
	};

	void updateCheckpoints() {
		m_fields->m_disabled = !m_isPracticeMode || (Mod::get()->getSettingValue<bool>("platformer-only") && !m_isPlatformer);
		if(m_fields->m_disabled) return;

		m_fields->m_opacities.clear();
		int pos = m_checkpointArray->count();
		for(auto checkpoint : CCArrayExt<CheckpointObject*>(m_checkpointArray)) {
			pos--;
			checkpoint->m_physicalCheckpointObject->m_isDisabled2 = false;
			checkpoint->m_physicalCheckpointObject->m_ignoreFade = true;
			m_fields->m_opacities[checkpoint] = std::ceil(Mod::get()->getSettingValue<int64_t>("default") * 2.55 * std::pow(Mod::get()->getSettingValue<double>("multiplier"), pos));
		}
	}

	$override
	void storeCheckpoint(CheckpointObject* p0) {
		PlayLayer::storeCheckpoint(p0);
		queueInMainThread([this]{ updateCheckpoints(); });
	}

	$override
	void removeCheckpoint(bool p0) {
		PlayLayer::removeCheckpoint(p0);
		queueInMainThread([this]{ updateCheckpoints(); });
	}

	$override
	void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);
		if(m_fields->m_disabled || m_checkpointArray->count() == 0) return;

		if(m_isPlatformer && !m_fields->m_checkpointNode) {
			m_fields->m_checkpointNode = static_cast<CheckpointObject*>(m_checkpointArray->lastObject())->m_physicalCheckpointObject->getParent();
			if(!m_fields->m_checkpointNode) return;
		}

		for(auto checkpoint : CCArrayExt<CheckpointObject*>(m_checkpointArray)) {
			checkpoint->m_physicalCheckpointObject->setOpacity(m_fields->m_opacities[checkpoint]);
			if(!m_isPlatformer || checkpoint->m_physicalCheckpointObject->getParent()) continue;
			m_fields->m_checkpointNode->addChild(checkpoint->m_physicalCheckpointObject);
			checkpoint->m_physicalCheckpointObject->setVisible(true);
		}
	}
};