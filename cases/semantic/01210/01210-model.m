(*

category:        Test
synopsis:        An assignment rule with an nary relational element.
componentTags:   AssignmentRule, Parameter
testTags:        InitialValueReassigned, UncommonMathML
testType:        TimeCourse
levels:          2.1, 2.2, 2.3, 2.4, 2.5, 3.1
generatedBy:     Analytic
packagesPresent: 

 In MathML, it is legal to have 3+ arguments to relational elements such as greater than.  This model checks to make sure interpreters notice the third argument, which changes the result of the 'piecewise' test.

The model contains:
* 1 parameter (x)

There is one rule:

[{width:30em,margin: 1em auto}|  *Type*  |  *Variable*  |  *Formula*  |
| Assignment | x | $piecewise(1, gt(2, 1, 2), 3)$ |]

The initial conditions are as follows:

[{width:35em,margin: 1em auto}|       | *Value* | *Constant* |
| Initial value of parameter x | $piecewise(1, gt(2, 1, 2), 3)$ | variable |]

Note: The test data for this model was generated from an analytical
solution of the system of equations.

*)