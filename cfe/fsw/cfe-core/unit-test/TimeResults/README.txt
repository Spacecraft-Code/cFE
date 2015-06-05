The TIME Services unit test results are expected to have the
following results for each of the TIME services source files:

==========================================================================

cfe_time_task.c - 100.00 % coverage

==========================================================================

cfe_time_tone.c - 100.00% coverage

==========================================================================

cfe_time_utils.c - 100.00% coverage

==========================================================================

cfe_time_api.c - 98.06% coverage (4 lines not executed)


CFE_TIME_CFE2FSSeconds

        3:  699:uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE)
        -:  700:{
        -:  701:    /*
        -:  702:    ** Using a signed integer allows the factor to be negative...
        -:  703:    */
        3:  704:    int32 ConvertFactor = CFE_TIME_FS_FACTOR;
        -:  705:
        -:  706:    /*
        -:  707:    ** File system time = cFE time + conversion factor...
        -:  708:    */
        3:  709:    uint32 SecondsFS = SecondsCFE + (uint32) ConvertFactor;
        -:  710:
        -:  711:    /*
        -:  712:    ** Prevent file system time from going below zero...
        -:  713:    */
        3:  714:    if (ConvertFactor < 0)
        -:  715:    {
    #####:  716:        if (-ConvertFactor > SecondsCFE)
        -:  717:        {
    #####:  718:            SecondsFS = 0;
        -:  719:        }
        -:  720:    }
        -:  721:
        3:  722:    return(SecondsFS);
        -:  723:
        -:  724:} /* End of CFE_TIME_CFE2FSSeconds() */

The reason these lines cannot be executued is because they are contingent on platform config
#define, and it is only set one way for a unit test.


CFE_TIME_Print

        -:  788:    /*
        -:  789:    ** Convert the cFE time (offset from epoch) into calendar time...
        -:  790:    */
       10:  791:    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_TIME_EPOCH_MINUTE;
       10:  792:    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_TIME_EPOCH_SECOND;
        -:  793:
        -:  794:    /*
        -:  795:    ** Adding the epoch "seconds" after computing the minutes avoids
        -:  796:    **    overflow problems when the input time value (seconds) is
        -:  797:    **    at, or near, 0xFFFFFFFF...
        -:  798:    */
       20:  799:    while (NumberOfSeconds >= 60)
        -:  800:    {
    #####:  801:        NumberOfMinutes++;
    #####:  802:        NumberOfSeconds -= 60;
        -:  803:    }

These lines are not executed either because of the 'while' being dependent on a platform config
#define


