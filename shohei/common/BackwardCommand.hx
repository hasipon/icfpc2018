package;
import haxe.ds.Vector;

enum BackwardCommand 
{
	Flip;
	Empty;
	LMove(position:Position);
	SMove(position:Position);
	Fission(target:BotId);
	Fusion(primary:BotId, secondary:BotId, primarySeeds:Array<BotId>, secondarySeeds:Array<BotId>);
	Fill(near:Near);
	SVoid(near:Near);
	GFill(top:Position, history:Vector<Vector<Vector<Bool>>>);
	GVoid(top:Position, history:Vector<Vector<Vector<Bool>>>);
	ReservFusionP;
	ReservFusionS;
}
