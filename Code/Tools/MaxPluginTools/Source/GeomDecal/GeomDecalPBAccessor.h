#pragma once

class GeomDecalPBAccessor : public PBAccessor
{
public:
	virtual void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) override;
};