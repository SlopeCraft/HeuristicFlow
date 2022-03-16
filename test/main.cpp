/*
 Copyright © 2022  TokiNoBug
This file is part of HeuristicFlow.

    HeuristicFlow is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HeuristicFlow is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HeuristicFlow.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <ctime>
#include "def_TestingGenetic.h"
#include "def_TestingPSO.h"
#include "def_TestingMatrix.h"
#include "def_TestingBoxes.h"

using namespace std;

int main() {
    //test_Box_double();
    testAckley_withRecord();
    //testNSGA2_Binh_and_Korn();
    //testNSGA3_DTLZ7();
    //searchPF();

    system("pause");
    return 0;
}
