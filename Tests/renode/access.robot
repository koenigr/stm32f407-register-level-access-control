*** Settings ***
Library         String
Library         Collections
Suite Setup     Suite Init
Suite Teardown  Teardown
Test Setup      Reset Emulation
Test Teardown   Test Teardown
Resource        ${RENODEKEYWORDS}

*** Variables ***
${REPO}         ${CURDIR}/../..
${ELF}          ${REPO}/build-firmware/stm32f407_access_control
${HOLD}         0.1
${GPIOD_ODR}    0x40020C14
${GREEN}        12
${RED}          14
&{KEYS}         1=0,0  2=0,1  3=0,2  4=1,0  5=1,1  6=1,2  7=2,0  8=2,1  9=2,2  0=3,1

*** Keywords ***
Suite Init
    Setup
    Execute Command    include @${REPO}/renode/KeypadMatrix.cs

Wait For Idle
    FOR    ${i}    IN RANGE    100
        ${odr}=    Execute Command    sysbus ReadDoubleWord ${GPIOD_ODR}
        ${imr}=    Execute Command    sysbus ReadDoubleWord 0x40013C00
        ${0}=    Convert To Integer    ${odr.strip()}
        ${m}=    Convert To Integer    ${imr.strip()}
        ${idle}=   Evaluate    (${o} & 0xF) == 0 and (${m} & 0xF00) == 0xF00
        IF    ${idle}
            RETURN
        END
        Execute Command    emulation RunFor "0.05"
    END
    Fail    Firmware did not return to idle

Prepare Machine
    Execute Command    mach create "access"
    Execute Command    machine LoadPlatformDescription @${REPO}/renode/stm32f407_access.repl
    Execute Command    sysbus LoadELF @${ELF}
    Execute Command    sysbus.cpu PerformanceInMips 16
    Execute Command    logLevel 3 sysbus
    ${sp}=    Execute Command    sysbus ReadDoubleWord 0x08000000
    ${pc}=    Execute Command    sysbus ReadDoubleWord 0x08000004
    Execute Command    sysbus.cpu VectorTableOffset 0x08000000
    Execute Command    sysbus.cpu SP ${sp.strip()}
    Execute Command    sysbus.cpu PC ${pc.strip()}
    Execute Command    emulation RunFor "0.1"

Tap Key
    [Arguments]    ${key}
    ${rc}=    Get From Dictionary    ${KEYS}    ${key}
    ${r}    ${c}=    Split String    ${rc}    ,
    Wait For Idle
    Execute Command    sysbus.keypad Press ${r} ${c}
    Execute Command    emulation RunFor "${HOLD}"
    Execute Command    sysbus.keypad Release ${r} ${c}
    Execute Command    emulation RunFor "${GAP}"
    Wait For Idle
    ${odr}=    Execute Command    sysbus ReadDoubleWord ${GPIOD_ODR}
    Log To Console    key ${key}: ODR=${odr.strip()}

Enter PIN
    [Arguments]    ${pin}
    @{digits}=    Get Regexp Matches    ${pin}    .
    FOR    ${d}    IN    @{digits}
        Tap Key    ${d}
    END

Led Is On
    [Arguments]    ${pin}
    ${out}=    Execute Command    sysbus ReadDoubleWord ${GPIOD_ODR}
    ${val}=    Convert To Integer    ${out.strip()}
    ${on}=     Evaluate    (${val} >> ${pin}) & 1 == 1
    RETURN    ${on}

Red Led Blinks
    ${seen}=    Set Variable    ${False}
    FOR    ${i}    IN RANGE    40
        Execute Command    emulation RunFor "0.05"
        ${on}=    Led Is On    ${RED}
        IF    ${on}    
            ${seen}=    Set Variable    ${True}
        END
        ${g}=    Led Is On    ${GREEN}
        Should Not Be True    ${g}
    END
    Should Be True    ${seen}

*** Test Cases ***
Correct PIN Unlocks
    Prepare Machine
    Enter PIN    1234
    ${g}=    Led Is On    ${GREEN}
    Should Be True    ${g}

Wrong PIN Stays Locked
    Prepare Machine
    Enter PIN    9999
    ${g}=    Led Is On    ${GREEN}
    Should Not Be True    ${g}

Three Wrong PINs Lock Out
    Prepare Machine
    FOR    ${i}    IN RANGE    3
        Enter PIN    0000
    END
    Red Led Blinks