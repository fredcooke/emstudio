###emstudio/test/*

---------
- __Introduction__<br>
  *and disclaimer:* `assert(unit_testing != integration_testing);`<br>
  This branch of the tree runs parallel to the core of emstudio and is in place to provide standalone [unit test](https://en.wikipedia.org/wiki/Unit_test) execution suitable for use with [CI](https://en.wikipedia.org/wiki/Continuous_integration#Principles) tools. Lets make emstudio as robust as possible with minimal effort.
- __General Guidelines and Rationale__<br>
  - 100% coverage in not a goal<br>
    I think it's noble, but not realistic. Large amounts of effort are usually expended on squeezing out those last few percent. Getting *everything* to 70% is more valuable than a few things to 98%.
  - Concentrate your efforts on testing the public methods of a class<br>
    Your goal is to verify the behavior of a class, not its implementation. If the interface is strong and fits the purpose well, then the actions behind the curtain can freely change. That said, there are times where you need to wrap some private/protected methods - but it should be an excessively rare.
  - No test overlap<br>
    Test one thing once. Your tests should be orthogonal. When a behavior changes, only one test *should* break.
  - You may find that individual test documentation is sparse or non-existant<br>
    If you find yourself writing lots of accompanying documentation for a particular test, maybe you're biting off more than you can chew. Maybe it's a poor design of the code under test. Dig a little deeper before moving on and possibly report it in the [issue tracker](http://issues.freeems.org/my_view_page.php) for followup and/or clarification.
  - Use `camelCase` for naming your tests<br>
  - Use `context_expectedResult` as the form of your test name<br>
    `invalidPath_returnsTrue`<br>
    `whenFull_notifiesParent`
  - Use an `ET_` prefix on your test classes and have their name mirror the class under test: `ET_EmsComms`<br>
    Helps with tags and sorting. Stands for (e)mstudio (t)est.
  - Expect pretty agressive refactoring of the tests themselves<br>
    If the test suite becomes a burden to use, no one will bother. Keep tests thin and confined to the task at hand.
- __Summary and Further Info__<br>
  If you've stuck with me thus far, you may understand where I was headed with my opening assertion. Unit tests are in place to validate and guarantee the integrity of components, not the application domain or vast chunks of it. Keep this in mind when contributing tests! If you have further questions or need clarification contact [me](mailto:sstasiak@gmail.com).
