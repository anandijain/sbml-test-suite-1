(* 

category:      Test
synopsis:      Basic single forward reaction with two species in one
               compartment using initialAssignment to set the initial value of one parameter.
componentTags: Compartment, Species, Reaction, Parameter, InitialAssignment 
testTags:      InitialAmount
testType:      TimeCourse
levels:        2.2, 2.3

The model contains one compartment called C.
  There are two species called S1 and S2 and two parameters called k1 and k2.
  The model contains one reaction defined as:

[{width:30em,margin-left:5em}|  *Reaction*  |  *Rate*  |
| S1 -> S2 | $C * k1 * S1$  |]

  The model contains one initialAssignment:

[{width:30em,margin-left:5em}| Variable | Formula |
 | k1 | $k2/100$  |]

  Note: InitialAssignments override any declared initial values.  In this case the 
initial value of the parameter is explicitly declared, but conflicts with the result 
of the initialAssignment.  The result of the initialAssignment should be used.


The initial conditions are as follows:

[{width:30em,margin-left:5em}| |  *Value*  |  *Units*  |
|              Initial amount of S1:| $            1$ | mole                      |
|              Initial amount of S2:| $          1.5$ | mole                      |
|             Value of parameter k1:| $ 100$ | second^-1^ |
|             Value of parameter k2:| $           50$ | second^-1^ |
| Volume of compartment C:| $             1$ | litre                     |]

The species values are given as amounts of substance to make it easier to
use the model in a discrete stochastic simulator, but (as per usual SBML
principles) they must be treated as concentrations where they appear in
expressions.

*)

newcase[ "00470" ];

addCompartment[ C, size -> 1];
addSpecies[ S1, initialAmount -> 1 ];
addSpecies[ S2, initialAmount -> 1.5 ];
addParameter[ k1, value -> 100, constant->False ];
addParameter[ k2, value -> 50 ];
addInitialAssignment[ k1, math -> k2/100];
addReaction[ S1 -> S2, reversible -> False,
	     kineticLaw -> C * k1 * S1];

makemodel[]
