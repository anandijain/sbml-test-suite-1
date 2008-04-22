(* 

category:      Test
synopsis:      Basic reaction with two species in a compartment using 
               initialAssignment to set the initial value of the compartment.
componentTags: Compartment, Species, Reaction, Parameter, InitialAssignment 
testTags:      InitialAmount
testType:      TimeCourse
levels:        2.2, 2.3

The model contains one compartment called C.
  There are three species called S1, S2 and S3 and three parameters called k1, k2 and p1.
  The model contains two reactions defined as:

[{width:30em,margin-left:5em}|  *Reaction*  |  *Rate*  |
| S1 + S2 -> S3 | $k1 * S1 * S2 * C$  |
| S3 -> S1 + S2 | $k2 * S3 * C$  |]

  The model contains one initialAssignment:

[{width:30em,margin-left:5em}| Variable | Formula |
 | C | $p1 * 2$  |]

  Note: InitialAssignments override any declared initial values.  In this case the 
initial value of the compartment is explicitly declared, and is consistent with the result 
of the initialAssignment. 

The initial conditions are as follows:

[{width:30em,margin-left:5em}| |  *Value*  |  *Units*  |
|              Initial amount of S1:| $1.0 \x 10^-15$ | mole                      |
|              Initial amount of S2:| $2.0 \x 10^-15$ | mole                      |
|              Initial amount of S3:| $1.0 \x 10^-15$ | mole                      |
|             Value of parameter k1:| $         0.75$ | litre mole^-1^ second^-1^ |
|             Value of parameter k2:| $0.25$ | second^-1^                |
|             Value of parameter p1:| $0.5$ | litre                |
|   Volume of compartment C:| $            1$ | litre                  |]

The species values are given as amounts of substance to make it easier to
use the model in a discrete stochastic simulator, but (as per usual SBML
principles) they must be treated as concentrations where they appear in
expressions.

*)

newcase[ "00480" ];

addCompartment[ C, size -> 1, constant -> False ];
addSpecies[ S1, initialAmount -> 1.0 10^-15];
addSpecies[ S2, initialAmount -> 2.0 10^-15];
addSpecies[ S3, initialAmount -> 1.0 10^-15];
addParameter[ k1, value -> 0.75 ];
addParameter[ k2, value -> 0.25 ];
addParameter[ p1, value -> 0.5 ];
addInitialAssignment[ C, math -> p1*2];
addReaction[ S1 + S2 -> S3, reversible -> False,
	     kineticLaw -> k1 * S1 * S2 * C ];
addReaction[ S3 -> S1 + S2, reversible -> False,
	     kineticLaw -> k2 * S3 * C ];

makemodel[]
