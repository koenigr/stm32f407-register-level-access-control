using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals;

namespace Antmicro.Renode.Peripherals.Miscellaneous
{
	public class KeypadMatrix : IGPIOReceiver
	{
		public KeypadMatrix()
		{
			Col0 = new GPIO(); Col1 = new GPIO(); Col2 = new GPIO(); Col3 = new GPIO();
			cols = new[] { Col0, Col1, Col2, Col3 };
			Reset();
		}

		public void OnGPIO(int number, bool value)
		{
			if(number < 0 || number > 3) return;
			rowLevel[number] = value;
			Update();
		}

		public void Reset()
		{
			for(var r = 0; r < 4; r++)
			{
				rowLevel[r] = true;
				for(var c = 0; c < 4; c++) pressed[r, c] = false;
			}
			Update();
		}

		public void Press(int row, int col)   { pressed[row, col] = true;  Update(); }
		public void Release(int row, int col) { pressed[row, col] = false; Update(); }

		private void Update()
		{
			for(var c = 0; c < 4; c++)
			{
				var low = false;
				for(var r = 0; r < 4; r++)
					if(pressed[r, c] && !rowLevel[r]) low = true;
				cols[c].Set(!low);
			}
		}

		public GPIO Col0 { get; private set; }
		public GPIO Col1 { get; private set; }
		public GPIO Col2 { get; private set; }
		public GPIO Col3 { get; private set; }

		private readonly GPIO[] cols;
		private readonly bool[] rowLevel = new bool[4];
		private readonly bool[,] pressed = new bool[4, 4];
	}
}