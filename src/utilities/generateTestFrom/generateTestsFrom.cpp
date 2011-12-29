/**
 * @file    printModel.cpp
 * @brief   Prints some information about the top-level model
 * @author  Sarah Keating
 * @author  Ben Bornstein
 * @author  Michael Hucka
 * 
 * This file is part of libSBML.  Please visit http://sbml.org for more
 * information about SBML, and the latest version of libSBML.
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <sstream>

#include <sbml/SBMLTypes.h>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

template <class T>
inline std::string toString (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

bool hasActualErrors(SBMLDocument* document)
{
  for (unsigned int e=0; e<document->getNumErrors(); e++) {
    if (document->getError(e)->getSeverity() >= LIBSBML_SEV_ERROR) return true;
  }
  return false;
}

vector<string> createTranslations(SBMLDocument* document, const string oldfilename)
{
  string filename = oldfilename;
  vector<string> ret;
  unsigned int level   = document->getLevel  ();
  unsigned int version = document->getVersion();
  
  string lxvx = "l" + toString(level) + "v" + toString(version);
  size_t lxvx_place = filename.find(lxvx);
  if (lxvx_place==string::npos) {
    cerr << "Error:  the filename '" << filename
         << "' doesn't have the substring '" << lxvx << "' in it." << endl;
    return ret;
  }

  //L1v2
  SBMLDocument* translatedDoc = document->clone();
  if (level==1 && version==2) {
    ret.push_back("1.2");
  }
  else {
    translatedDoc->setLevelAndVersion(1, 2);
    if (!hasActualErrors(translatedDoc)) {
      string newfilename = filename.replace(lxvx_place,4,"l1v2");
      if (newfilename == oldfilename) {
        ret.push_back("1.2");
      }
      else if (writeSBMLToFile(translatedDoc, newfilename.c_str())==1) {
        cout << "Successfully wrote translation of model to level 1 version 2" << endl;
        ret.push_back("1.2");
      }  
    }
    delete translatedDoc;
  }
  //L2v*
  for (int v=1; v<5; v++) {
    if (level==2 && version==v) {
      ret.push_back("2." + toString(v));
    }
    else {
      translatedDoc = document->clone();
      translatedDoc->setLevelAndVersion(2, v);
      if (!hasActualErrors(translatedDoc)) {
        string thislv = "l2v" + toString(v);
        string newfilename = filename.replace(lxvx_place,4,thislv);
        if (newfilename == oldfilename) { 
          ret.push_back("2." + toString(v));
        }
        else if (writeSBMLToFile(translatedDoc, newfilename.c_str())==1) {
          cout << "Successfully wrote translation of model to level 2 version " << v << endl;
          ret.push_back("2." + toString(v));
        }
      }
      delete translatedDoc;
    }
  }

  //L3v1
  if (level==2 && version==1) {
    ret.push_back("3.1");
  }
  else {
    translatedDoc = document->clone();
    translatedDoc->setLevelAndVersion(3, 1);
    if (!hasActualErrors(translatedDoc)) {
      string newfilename = filename.replace(lxvx_place,4,"l3v1");
      if (newfilename == oldfilename) {
        ret.push_back("3.1");
      }
      else if (writeSBMLToFile(translatedDoc, newfilename.c_str())==1) {
        cout << "Successfully wrote translation of model to level 3 version 1" << endl;
        ret.push_back("3.1");
      }  
    }
    delete translatedDoc;
  }
  return ret;
}

string getString(vector<string> input)
{
  string ret = "";
  for (size_t i=0; i<input.size(); i++) {
    if (i>0) ret += ", ";
    ret += input[i];
  }
  return ret;
}

string getString(set<string> input)
{
  string ret = "";
  for (set<string>::iterator i=input.begin(); i != input.end(); i++) {
    if (i != input.begin()) ret += ", ";
    ret += *i;
  }
  return ret;
}

string getString(bool input)
{
  if (input) return "true";
  return "false";
}

bool variesIn(string id, Model* model)
{
  if (model->getRule(id)) return true;
  for (unsigned long e=0; e<model->getNumEvents(); e++) {
    Event* event = model->getEvent(e);
    for (unsigned long ea=0; ea<event->getNumEventAssignments(); ea++) {
      if (event->getEventAssignment(ea)->getVariable() == id) return true;
    }
  }
  return false;
}

bool initialOverriddenIn(string id, Model* model)
{
  if (model->getInitialAssignment(id) != NULL) return true;
  Rule* rule = model->getRule(id);
  if (rule==NULL) return false;
  if (rule->getTypeCode()==SBML_ASSIGNMENT_RULE) return true;
  return false;
}

void checkRules(Model* model, set<string>& components, set<string>& tests)
{
  for (unsigned int r=0; r<model->getNumRules(); r++) {
    Rule* rule = model->getRule(r);
    int typecode = rule->getTypeCode();
    if (typecode == SBML_ALGEBRAIC_RULE) {
      components.insert("AlgebraicRule");
    }
    else if (typecode == SBML_ASSIGNMENT_RULE) {
      components.insert("AssignmentRule");
    }
    else if (typecode == SBML_RATE_RULE) {
      components.insert("RateRule");
    }
  }
}

void checkCompartments(Model* model, set<string>& components, set<string>& tests)
{
  if (model->getNumCompartments() > 0) {
    components.insert("Compartment");
    if (model->getNumCompartments() > 1) {
      tests.insert("MultiCompartment");
    }
    for (unsigned int c=0; c<model->getNumCompartments(); c++) {
      Compartment* compartment = model->getCompartment(c);
      if (compartment->isSetId() && variesIn(compartment->getId(), model)) {
        tests.insert("NonConstantCompartment");
        tests.insert("NonUnityCompartment");
      }
      else if (compartment->isSetVolume()) {
        double volume = compartment->getVolume();
        if (volume != 1.0) {
          tests.insert("NonUnityCompartment");
        }
      }
    }
  }
}

void checkEvents(Model* model, set<string>& components, set<string>& tests)
{
  for (unsigned int e=0; e<model->getNumEvents(); e++) {
    Event* event = model->getEvent(e);
    if (event->isSetDelay()) {
      components.insert("EventDelay");
    }
    else {
      components.insert("EventNoDelay");
    }
    if (event->isSetPriority()) {
      components.insert("EventPriority");
    }
    if (event->isSetTrigger()) {
      Trigger* trigger = event->getTrigger();
      if (event->isSetDelay() || model->getNumEvents() > 2) {
        if (trigger->isSetPersistent()) {
          if (trigger->getPersistent()) {
            tests.insert("EventIsPersistent [?]");
          }
          else {
            tests.insert("EventIsNotPersistent [?]");
          }
        }
        if (event->isSetUseValuesFromTriggerTime()) {
          if (event->getUseValuesFromTriggerTime()) {
            tests.insert("EventUsesTriggerTimeValues [?]");
          }
          else {
            tests.insert("EventUsesAssignmentTimeValues [?]");
          }
        }
      }
      if (trigger->isSetInitialValue() && trigger->getInitialValue()==false) {
        tests.insert("EventT0Firing [?]");
      }
    }
  }
}

void checkParameters(Model* model, set<string>& components, set<string>& tests)
{
  if (model->getNumParameters() > 0) {
    components.insert("Parameter");
    for (unsigned int p=0; p<model->getNumParameters(); p++) {
      Parameter* param = model->getParameter(p);
      if (param->isSetValue() && param->isSetId() && 
          initialOverriddenIn(param->getId(), model)) {
        tests.insert("InitialValueReassigned");
      }
      if (param->isSetId() && variesIn(param->getId(), model)) {
        tests.insert("NonConstantParameter");
      }
    }
  }
}

void checkReactions(Model* model, set<string>& components, set<string>& tests)
{
  if (model->getNumReactions() > 0) {
    components.insert("Reaction");
    for (unsigned int r=0; r<model->getNumReactions(); r++) {
      Reaction* rxn = model->getReaction(r);
      if (rxn->isSetFast() && rxn->getFast()) {
        tests.insert("FastReaction");
      }
      for (unsigned int rp=0; rp<rxn->getNumReactants(); rp++) {
        SpeciesReference* sr = rxn->getReactant(rp);
        bool assignedstoich = false;
        if (sr->isSetStoichiometryMath()) {
          assignedstoich = true;
        }
        else if ((model->getLevel()==3) && sr->isSetId() && variesIn(sr->getId(), model)) {
          assignedstoich = true;
        }
        if (assignedstoich) {
          if (sr->getConstant()) {
            tests.insert("AssignedConstantStoichiometry");
          }
          else {
            tests.insert("AssignedVariableStoichiometry");
          }
        }
      }
      for (unsigned int rp=0; rp<rxn->getNumProducts(); rp++) {
        SpeciesReference* sr = rxn->getProduct(rp);
        bool assignedstoich = false;
        if (sr->isSetStoichiometryMath()) {
          assignedstoich = true;
        }
        else if (model->getLevel()==3 && sr->isSetId() && variesIn(sr->getId(), model)) {
          assignedstoich = true;
        }
        if (assignedstoich) {
          if (sr->getConstant()) {
            tests.insert("AssignedConstantStoichiometry");
          }
          else {
            tests.insert("AssignedVariableStoichiometry");
          }
        }
      }
      if (rxn->isSetKineticLaw()) {
        KineticLaw* kl = rxn->getKineticLaw();
        if (kl->getNumLocalParameters() > 0) {
          tests.insert("LocalParameters");
        }
      }
    }
  }
}

void checkSpecies(Model* model, set<string>& components, set<string>& tests)
{
  //Must call this after 'checkCompartments' because we look in 'tests' for 'NonUnityCompartment'.
  if (model->getNumSpecies() > 0) {
    components.insert("Species");
    tests.insert("Amount||Concentration");
    for (unsigned int s=0; s<model->getNumSpecies(); s++) {
      Species* species = model->getSpecies(s);
      if (species->isSetBoundaryCondition() && species->getBoundaryCondition()) {
        tests.insert("BoundaryCondition");
      }
      if (species->isSetConstant() && species->getConstant()) {
        tests.insert("ConstantSpecies");
      }
      if (species->isSetConversionFactor()) {
        tests.insert("ConversionFactors");
      }
      if (species->isSetHasOnlySubstanceUnits() && species->getHasOnlySubstanceUnits()
          && tests.find("NonUnityCompartment") != tests.end()) {
        tests.insert("HasOnlySubstanceUnits");
      }
      if ((species->isSetInitialAmount() || species->isSetInitialConcentration()) &&
          species->isSetId() && variesIn(species->getId(), model)) {
        tests.insert("InitialValueReassigned");
      }
    }
  }
}

vector<string> getIdListFrom(ListOf* list)
{
  vector<string> ret;
  for (unsigned int l=0; l<list->size(); l++) {
    ret.push_back(list->get(l)->getId());
  }
  return ret;
}

string getHalfReaction(ListOfSpeciesReferences* srs, vector<string>& stoichrefs, Model* model)
{
  stringstream ret;
  for (unsigned int s = 0; s<srs->size(); s++) {
    if (s > 0) ret << "+ ";
    SpeciesReference* sr = static_cast<SpeciesReference*>(srs->get(s));
    string species = sr->getSpecies();
    if (sr->isSetStoichiometryMath()) {
      if (sr->isSetId()) {
        ret << sr->getId() << " ";
        stoichrefs.push_back(sr->getId());
      }
      else {
        ret << species << "_ext ";
        stoichrefs.push_back(species + "_ext");
      }
    }
    else if (sr->isSetId() && (initialOverriddenIn(sr->getId(), model) || variesIn(sr->getId(), model))) {
      ret << sr->getId() << " ";
      stoichrefs.push_back(sr->getId());
    }
    else if (sr->isSetStoichiometry() && sr->getStoichiometry() != 1.0) {
      ret << sr->getStoichiometry();
    }
    ret << species << " ";
  }
  return ret.str();
}

string getReactionTable(Model* model)
{
  stringstream ret;
  bool anyfast = false;
  for (unsigned int r=0; r<model->getNumReactions(); r++) {
    if (model->getReaction(r)->getFast()) anyfast = true;
  }
  vector<string> stoichrefs;
  unsigned int numrxns = model->getNumReactions();
  if (numrxns > 0) {
    ret << endl << "There ";
    if (numrxns>1) {
      ret << "are " << numrxns << " reactions:";
    }
    else {
      ret << "is one reaction:";
    }
    ret << endl << endl;
    ret << "[{width:30em,margin-left:5em}|  *Reaction*  |  *Rate*  |";
    if (anyfast) {
      ret << "  *Fast*  |";
    }
    for (unsigned int r=0; r<model->getNumReactions(); r++) {
      Reaction* rxn = model->getReaction(r);
      ret  << endl<< "| ";
      ret << getHalfReaction(rxn->getListOfReactants(), stoichrefs, model);
      ret << "-> ";
      ret << getHalfReaction(rxn->getListOfProducts(), stoichrefs, model);
      ret << "| ";
      if (rxn->isSetKineticLaw()) {
        ret << "$" << SBML_formulaToString(rxn->getKineticLaw()->getMath()) << "$";
      }
      else {
        ret << "$(not set)$";
      }
      ret << " |";
      if (anyfast) {
        ret << " ";
        if (rxn->getFast()) {
          ret << "fast";
        }
        else {
          ret << "slow";
        }
        ret << " |";
      }
    }
    ret << "]" << endl;
    string srlist = getString(stoichrefs);
    if (!srlist.empty()) {
      ret << "Note:  the following stoichiometries are set separately:  " << srlist << endl;
    }
    ret << endl;
  }
  return ret.str();
}

void setEventFlags(Model* model, bool& priority, bool& persistent, bool& t0, bool& assignmenttime, bool& delay)
{
  for (unsigned int e=0; e<model->getNumEvents(); e++) {
    Event* event = model->getEvent(e);
    if (event->isSetPriority()) {
      priority = true;
    }
    if (event->isSetDelay()) {
      delay = true;
    }
    if (event->isSetUseValuesFromTriggerTime() && event->getUseValuesFromTriggerTime()==false) {
      assignmenttime = true;
    }
    Trigger* trigger = event->getTrigger();
    if (trigger->isSetInitialValue() && trigger->getInitialValue()==false) {
      t0 = true;
    }
    if (trigger->isSetPersistent() && trigger->getPersistent()==false) {
      persistent = true;
    }
  }
}

string getEventTable(Model* model)
{
  stringstream ret;
  unsigned int numevents = model->getNumEvents();
  if ( numevents > 0) {
    bool priority(false), persistent(false), t0(false), assignmenttime(false), delay(false);
    setEventFlags(model, priority, persistent, t0, assignmenttime, delay);
    ret << endl << "There ";
    if (numevents>1) {
      ret << "are " << numevents << " events:";
    }
    else {
      ret << "is one event:";
    }
    ret << endl << endl;
    string topline = "|  *Event*  |  *Trigger*  |";
    int numextras = 0;
    if (priority) {
      topline += "  *Priority*  |";
      numextras++;
    }
    if (persistent) {
      topline += "  *Persistent*  |";
      numextras++;
    }
    if (t0) {
      topline += "  *initialValue*  |";
      numextras++;
    }
    if (assignmenttime) {
      topline += "  *Use values from:*  |";
      numextras++;
    }
    if (delay) {
      topline += "  *Delay*  |";
      numextras++;
    }
    topline += " *Event Assignments* |";
    ret << "[{width:" << 30 + numextras*5 << "em,margin-left:5em}" << topline;
    for (unsigned int e = 0; e<model->getNumEvents(); e++) {
      ret << endl << "| ";
      Event* event = model->getEvent(e);
      if (event->isSetId()) ret << event->getId();
      ret << " | ";
      ret << "$" << SBML_formulaToString(event->getTrigger()->getMath()) << "$ | ";
      if (priority) {
        if (event->isSetPriority()) ret << "$" << SBML_formulaToString(event->getPriority()->getMath()) << "$";
        else ret << "(unset)";
        ret << " | ";
      }
      if (persistent) {
        ret << getString(event->getTrigger()->getPersistent()) << " | ";
      }
      if (t0) {
        ret << getString(event->getTrigger()->getInitialValue()) << " | ";
      }
      if (assignmenttime) {
        if (event->getUseValuesFromTriggerTime()) {
          ret << "Trigger time | ";
        }
        else {
          ret << "Assignment time | ";
        }
      }
      if (delay) {
        if (event->isSetDelay()) ret << "$" << SBML_formulaToString(event->getDelay()->getMath()) << "$ | ";
        else ret << "$0$ | ";
      }
      if (event->getNumEventAssignments()==0) {
        ret << " |";
      }
      else {
        for (unsigned long e=0; e<event->getNumEventAssignments(); e++) {
          if (e > 0) {
            ret << endl << "|  |  | ";
            for (int pipes = 0; pipes<numextras; pipes++) {
              ret << " | ";
            }
          }
          EventAssignment* ea = event->getEventAssignment(e); 
          ret << "$" << ea->getVariable() << " = " << SBML_formulaToString(ea->getMath()) << "$ |";
        }
      }
    }
    ret << "]" << endl << endl;
  }
  return ret.str();
}

string getRuleTable(Model* model)
{
  stringstream ret;
  unsigned int numrules = model->getNumRules();
  if (numrules > 0) {
    ret << endl << "There ";
    if (numrules>1) {
      ret << "are " << numrules << " rules:";
    }
    else {
      ret << "is one rule:";
    }
    ret << endl << endl;
    ret << "[{width:30em,margin-left:5em}|  *Type*  |  *Variable*  |  *Formula*  |";
    for (unsigned int r=0; r<model->getNumRules(); r++) {
      Rule* rule = model->getRule(r);
      ret << endl << "| ";
      string var = rule->getVariable();
      switch(rule->getTypeCode()) {
      case SBML_ALGEBRAIC_RULE:
        ret << "Algebraic";
        var = "$0$";
        break;
      case SBML_RATE_RULE:
        ret << "Rate";
        break;
      case SBML_ASSIGNMENT_RULE:
        ret << "Assignment";
        break;
      }
      ret << " | " << var << " | $" << SBML_formulaToString(rule->getMath()) << "$ |";
    }
    ret << "]" << endl;
    ret << endl;
  }
  return ret.str();
}

string getInitialSpeciesLevels(Model* model, bool isconst)
{
  stringstream ret;
  for (unsigned long s=0; s<model->getNumSpecies(); s++) {
    Species* species = model->getSpecies(s);
    if (species->getConstant() != isconst) continue;
    string id = species->getId();
    Rule* rule = model->getRule(id);
    InitialAssignment* ia = model->getInitialAssignment(id);
    ret << endl << "| ";
    if (rule != NULL || ia != NULL) {
      if (species->getHasOnlySubstanceUnits()) {
        ret << "Initial amount of species ";
      }
      else {
        ret << "Initial concentration of species ";
      }
      ret << id << " | $"; 
      if (rule != NULL) {
        ret << SBML_formulaToString(rule->getMath()) << "$ |";
      }
      else {
        ret << SBML_formulaToString(ia->getMath()) << "$ |";
      }
    }
    else if (species->isSetInitialAmount()) {
      ret << "Initial amount of species " << id << " | $" << species->getInitialAmount() << "$ |";
    }
    else if (species->isSetInitialConcentration()) {
      ret << "Initial concentration of species " << id << " | $" << species->getInitialConcentration() << "$ |";
    }
    else {
      ret << "Initial level of species " << id << " | $unknown$ |";
    }
    if (isconst) {
      ret << " constant |";
    }
    else {
      ret << " variable |";
    }
  }
  return ret.str();
}

string getInitialParameterLevels(Model* model, bool isconst)
{
  stringstream ret;
  for (unsigned long p=0; p<model->getNumParameters(); p++) {
    Parameter* param = model->getParameter(p);
    if (param->getConstant() != isconst) continue;
    string id = param->getId();
    ret << endl << "| Initial value of parameter " << id << " | $"; 
    Rule* rule = model->getRule(id);
    InitialAssignment* ia = model->getInitialAssignment(id);
    if (rule != NULL || ia != NULL) {
      if (rule != NULL) {
        ret << SBML_formulaToString(rule->getMath()) << "$ |";
      }
      else {
        ret << SBML_formulaToString(ia->getMath()) << "$ |";
      }
    }
    else if (param->isSetValue()) {
      ret << param->getValue() << "$ |";
    }
    else {
      ret << "unknown$ |";
    }
    if (isconst) {
      ret << " constant |";
    }
    else {
      ret << " variable |";
    }
  }
  return ret.str();
}

string getInitialCompartmentLevels(Model* model, bool isconst)
{
  stringstream ret;
  for (unsigned long c=0; c<model->getNumCompartments(); c++) {
    Compartment* compartment = model->getCompartment(c);
    if (compartment->getConstant() != isconst) continue;
    string id = compartment->getId();
    ret << endl << "| Initial volume of compartment '" << id << "' | $"; 
    Rule* rule = model->getRule(id);
    InitialAssignment* ia = model->getInitialAssignment(id);
    if (rule != NULL || ia != NULL) {
      if (rule != NULL) {
        ret << SBML_formulaToString(rule->getMath()) << "$ |";
      }
      else {
        ret << SBML_formulaToString(ia->getMath()) << "$ |";
      }
    }
    else if (compartment->isSetVolume()) {
      ret << compartment->getVolume() << "$ |";
    }
    else {
      ret << "unknown$ |";
    }
    if (isconst) {
      ret << " constant |";
    }
    else {
      ret << " variable |";
    }
  }
  return ret.str();
}

string getInitialAssignmentTable(Model* model)
{
  stringstream ret;
  if (model->getNumParameters() + model->getNumSpecies() + model->getNumCompartments() == 0) return "";
  ret << "The initial conditions are as follows:" << endl << endl;
  ret << "[{width:35em,margin-left:5em}|       | *Value* | *Constant* |";
  ret << getInitialSpeciesLevels(model, true);
  ret << getInitialSpeciesLevels(model, false);
  ret << getInitialParameterLevels(model, true);
  ret << getInitialParameterLevels(model, false);
  ret << getInitialCompartmentLevels(model, true);
  ret << getInitialCompartmentLevels(model, false);
  ret << "]" << endl;
  return ret.str();
}

bool allSpeciesSetAmountUsedConcentration(Model* model)
{
  if (model->getNumSpecies() == 0) return false;
  for (unsigned int s=0; s<model->getNumSpecies(); s++) {
    Species* sp = model->getSpecies(s);
    if (!sp->isSetInitialAmount()) return false;
    if (sp->getHasOnlySubstanceUnits()) return false;
  }
  return true;
}

string getModelSummary(Model* model) 
{
  stringstream ret;
  ret << "The model contains:\n";
  vector<string> namelist;
  namelist = getIdListFrom(model->getListOfSpecies());
  if (namelist.size()>0) {
    ret << "* " << namelist.size() << " species (" << getString(namelist) << ")\n";
  }
  namelist = getIdListFrom(model->getListOfParameters());
  if (namelist.size()>0) {
    ret << "* " << namelist.size() << " parameter";
    if (namelist.size() > 1) ret << "s";
    ret << " (" << getString(namelist) << ")\n";
  }
  namelist = getIdListFrom(model->getListOfCompartments());
  if (namelist.size()>0) {
    ret << "* " << namelist.size() << " compartment";
    if (namelist.size() > 1) ret << "s";
    ret << " (" << getString(namelist) << ")\n";
  }

  if (model->getNumConstraints() > 0 || model->getNumFunctionDefinitions() > 0) {
    ret << endl << "It also contains ";
    if (model->getNumConstraints() > 0) {
      ret << model->getNumConstraints() << " constraints (";
      for (unsigned int c=0; c<model->getNumConstraints(); c++) {
        if (c>1) ret << ", ";
        ret << SBML_formulaToString(model->getConstraint(c)->getMath());
      }
      ret << ") ";
      if (model->getNumFunctionDefinitions() > 0) ret << "and ";
    }
    if (model->getNumFunctionDefinitions() > 0) {
      ret << model->getNumFunctionDefinitions() << " function definition(s):\n";
      for (unsigned int fd=0; fd<model->getNumFunctionDefinitions(); fd++) {
        const FunctionDefinition* func = model->getFunctionDefinition(fd);
        ret << "; " << func->getId() << ": $" 
            << SBML_formulaToString(func->getMath()->getRightChild()) << "$" << endl;
      }
    }
  }

  ret << getReactionTable(model);
  ret << getEventTable(model);
  ret << getRuleTable(model);
  ret << getInitialAssignmentTable(model);
  ret << endl;

  if (allSpeciesSetAmountUsedConcentration(model)) {
    ret << "The species' initial quantities are given in terms of substance units to" << endl;
    ret << "make it easier to use the model in a discrete stochastic simulator, but" << endl;
    ret << "their symbols represent their values in concentration units where they" << endl;
    ret << "appear in expressions." << endl << endl;
  }

  ret << "{Keep this next line if 'generatedBy' is 'Analytic':}" << endl;
  ret << "Note: The test data for this model was generated from an analytical" << endl;
  ret << "solution of the system of equations." << endl;

  return ret.str();
}

string getSuiteHeaders(vector<string> levelsandversions, Model* model)
{
  set<string> components;
  set<string> tests;
  checkRules(model, components, tests);
  checkCompartments(model, components, tests);
  checkEvents(model, components, tests);
  if (model->getNumFunctionDefinitions() > 0) {
    components.insert("FunctionDefinition");
  }
  if (model->getNumInitialAssignments() > 0) {
    components.insert("InitialAssignment");
  }
  checkParameters(model, components, tests);
  checkReactions(model, components, tests);
  checkSpecies(model, components, tests);
  if (model->isSetConversionFactor()) {
    tests.insert("ConversionFactors");
  }
  string modxml = model->toSBML();
  if (modxml.find("http://www.sbml.org/sbml/symbols/avogadro") != string::npos) {
    components.insert("CSymbolAvogadro");
  }
  if (modxml.find("http://www.sbml.org/sbml/symbols/delay") != string::npos) {
    components.insert("CSymbolDelay");
  }
  if (modxml.find("http://www.sbml.org/sbml/symbols/time") != string::npos) {
    components.insert("CSymbolTime");
  }


  string ret = "";
  ret += "category:      Test\n";
  ret += "synopsis:      [[Write description here.]]\n";
  ret += "componentTags: " + getString(components) + "\n";
  ret += "testTags:      " + getString(tests) + "\n";
  ret += "testType:      TimeCourse\n";
  ret += "levels:        " + getString(levelsandversions) + "\n";
  ret += "generatedBy:   Analytic||Numeric\n";
  ret += "\n";
  ret += "{Write general description of why you have created the model here.}\n";
  ret += "\n";
  return ret;
}

void
writeMFile(string contents, string modfilename)
{
  size_t sbml_place = modfilename.find("-sbml-l");
  if (sbml_place==string::npos) {
    cerr << "Error:  the filename '" << modfilename
         << "' doesn't have the substring '-sbml-l' in it.  Can't write .m file" << endl;
    return;
  }
  string filename = modfilename.replace(sbml_place, 14, "-model.m");
  ifstream infile(filename);
  string oldfile(""), tmp;
  while (!infile.eof() && infile.good()) {
    getline(infile, tmp);
    oldfile += tmp + "\n";
  }
  ofstream file(filename);
  file << contents << endl;
  if (!oldfile.empty()) {
    file << "/*" << endl
         << "Previous version of this file:  " << endl
         << oldfile << endl
         << "*/" << endl;
  }
  file.close();
  cout << "Successfully wrote model description file " << modfilename << endl;
}

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << endl << "Usage: generateTestsFrom filename" << endl << endl;
    return 1;
  }

  string filename   = argv[1];
  SBMLDocument* document = readSBML(filename.c_str());

  if (hasActualErrors(document))
  {
    cerr << "Encountered the following SBML errors:" << endl;
    document->printErrors(cerr);
    return 1;
  }

  Model* model = document->getModel();

  if (model == NULL)
  {
    cout << "No model present." << endl;
    return 1;
  }

  vector<string> levelsandversions = createTranslations(document, filename);
  string mfile = "(*\n\n";
  mfile += getSuiteHeaders(levelsandversions, model);
  mfile += getModelSummary(model);
  mfile += "\n*)";
  writeMFile(mfile, filename);


  delete document;
  return 0;
}