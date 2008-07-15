(* 

category:      Test
synopsis:      Basic reaction and rate rule with three species in a 1D compartment. 
componentTags: Compartment, Species, Reaction, Parameter, RateRule 
testTags:      InitialAmount, 1D-Compartment
testType:      TimeCourse
levels:        2.1, 2.2, 2.3
generatedBy:   Numeric

The model contains one compartment called "compartment".  There are three
species named S1, S2 and S3 and one parameter named k1.  Compartment
"compartment" is 1-dimensional.  The model contains one reaction defined
as:

[{width:30em,margin-left:5em}|  *Reaction*  |  *Rate*  |
| S1 -> S2 | $k1 * S1 * compartment$  |]

The model contains one rule:

[{width:30em,margin-left:5em}|  *Type*  |  *Variable*  |  *Formula*  |
 | Rate | S3 | $0.65$  |]


The initial conditions are as follows:

[{width:30em,margin-left:5em}|       |*Value*          |*Units*  |
|Initial amount of S1                |$          1.5$ |mole                      |
|Initial amount of S2                |$            0$ |mole                      |
|Initial amount of S3                |$            0$ |mole                      |
|Value of parameter k1               |$        1.546$ |second^-1^ |
|length of compartment "compartment" |$            1$ |metre                     |]

The species values are given as amounts of substance to make it easier to
use the model in a discrete stochastic simulator, but (as per usual SBML
principles) their symbols represent their values in concentration units
where they appear in expressions.

*)

newcase[ "00320" ];

addCompartment[ compartment, spatialDimensions -> 1, size -> 1 ];
addSpecies[ S1, initialAmount -> 1.5 ];
addSpecies[ S2, initialAmount -> 0 ];
addSpecies[ S3, initialAmount -> 0 ];
addParameter[ k1, value -> 1.546 ];
addRule[ type->RateRule, variable -> S3, math -> 0.65];
addReaction[ S1 -> S2, reversible -> False,
	     kineticLaw -> k1 * S1 * compartment ];

makemodel[]