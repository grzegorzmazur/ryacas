

#include "yacas/yacasprivate.h"
#include "yacas/lispevalhash.h"
#include "yacas/lispenvironment.h"
#include "yacas/lispatom.h"
#include "yacas/lispeval.h"
#include "yacas/errors.h"



void YacasEvaluator::Evaluate(LispPtr& aResult,LispEnvironment& aEnvironment,LispPtr& aArguments)
{

  if (!(iFlags & Variable))
  {
    CheckNrArgs(iNrArgs+1,aArguments,aEnvironment);
  }

  LispInt stacktop = aEnvironment.iStack.GetStackTop();

  // Push a place holder for the result: push full expression so it is available for error reporting
  aEnvironment.iStack.PushArgOnStack(aArguments);

  LispIterator iter(aArguments);
  ++iter;

  LispInt i;
  LispInt nr = iNrArgs;

  if (iFlags & Variable) nr--;

  // Walk over all arguments, evaluating them as necessary
  if (iFlags & Macro)
  {
    for (i=0;i<nr;i++)
    {
      if (!iter.getObj())
          throw LispErrWrongNumberOfArgs();

      aEnvironment.iStack.PushArgOnStack(iter.getObj()->Copy());
      ++iter;
    }
    if (iFlags & Variable)
    {
      LispPtr head(aEnvironment.iList->Copy());
      head->Nixed() = (iter.getObj());
      aEnvironment.iStack.PushArgOnStack(LispSubList::New(head));
    }
  }
  else
  {
    LispPtr arg;
    for (i=0;i<nr;i++)
    {
      if (!iter.getObj())
          throw LispErrWrongNumberOfArgs();

      aEnvironment.iEvaluator->Eval(aEnvironment, arg, *iter);
      aEnvironment.iStack.PushArgOnStack(arg);
      ++iter;
    }
    if (iFlags & Variable)
    {

//LispString res;

//printf("Enter\n");
      LispPtr head(aEnvironment.iList->Copy());
      head->Nixed() = (iter.getObj());
      LispPtr list(LispSubList::New(head));


/*
PrintExpression(res, list,aEnvironment,100);
printf("before %s\n",res.String());
*/

      aEnvironment.iEvaluator->Eval(aEnvironment, arg, list);

/*
PrintExpression(res, arg,aEnvironment,100);
printf("after %s\n",res.String());
*/

      aEnvironment.iStack.PushArgOnStack(arg);
//printf("Leave\n");
    }
  }

  iCaller(aEnvironment,stacktop);
  aResult = (aEnvironment.iStack.GetElement(stacktop));
  aEnvironment.iStack.PopTo(stacktop);
}


