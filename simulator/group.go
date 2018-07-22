package main

type GroupManager struct{}

func NewGroupManager() *GroupManager {
	return &GroupManager{}
}

func (g *GroupManager) AddCommand(r Region) {}

func (g *GroupManager) Valid() bool {
	return true
}

func (g *GroupManager) Regions() []Region {
	return []Region{}
}
