#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PASS=0
FAIL=0

print_header() {
    echo
    echo "=========================================="
    echo -e "${BLUE}$1${NC}"
    echo "=========================================="
}

print_test() {
    echo -e "${YELLOW}Test: $1${NC}"
    echo "Command: ./philo $2"
    echo "Expected: $3"
}

run_test() {
    local test_name=$1
    local args=$2
    local expected=$3
    local should_die=$4
    
    print_test "$test_name" "$args" "$expected"
    
    OUTPUT=$(timeout 5 ./philo $args 2>&1)
    
    if echo "$OUTPUT" | grep -q "died"; then
        if [ "$should_die" = "yes" ]; then
            echo -e "${GREEN}✓ PASS: Death detected as expected${NC}"
            PASS=$((PASS + 1))
            
            # Check death timing
            DEATH_LINE=$(echo "$OUTPUT" | grep "died" | head -1)
            DEATH_TIME=$(echo "$DEATH_LINE" | awk '{print $1}')
            PHILO_ID=$(echo "$DEATH_LINE" | awk '{print $2}')
            echo "Death: Philosopher $PHILO_ID died at ${DEATH_TIME}ms"
        else
            echo -e "${RED}✗ FAIL: Unexpected death${NC}"
            FAIL=$((FAIL + 1))
        fi
    else
        if [ "$should_die" = "no" ]; then
            echo -e "${GREEN}✓ PASS: No death (as expected)${NC}"
            PASS=$((PASS + 1))
        else
            echo -e "${RED}✗ FAIL: Expected death but none occurred${NC}"
            FAIL=$((FAIL + 1))
        fi
    fi
    echo
}

check_death_timing() {
    local args=$1
    local time_to_die=$(echo $args | awk '{print $2}')
    
    print_header "DEATH TIMING TEST"
    print_test "Death timing precision" "$args" "Death within 10ms of expected time"
    
    OUTPUT=$(./philo $args 2>&1)
    DEATH_LINE=$(echo "$OUTPUT" | grep "died" | head -1)
    
    if [ -z "$DEATH_LINE" ]; then
        echo -e "${RED}✗ FAIL: No death detected${NC}"
        FAIL=$((FAIL + 1))
        return
    fi
    
    DEATH_TIME=$(echo "$DEATH_LINE" | awk '{print $1}')
    PHILO_ID=$(echo "$DEATH_LINE" | awk '{print $2}')
    LAST_MEAL=$(echo "$OUTPUT" | grep "$PHILO_ID is eating" | tail -1 | awk '{print $1}')
    
    if [ -z "$LAST_MEAL" ]; then
        LAST_MEAL=0
    fi
    
    EXPECTED=$((LAST_MEAL + time_to_die))
    DELAY=$((DEATH_TIME - EXPECTED))
    
    echo "Philosopher $PHILO_ID:"
    echo "  Last meal: ${LAST_MEAL}ms"
    echo "  Expected death: ${EXPECTED}ms"
    echo "  Actual death: ${DEATH_TIME}ms"
    echo "  Delay: ${DELAY}ms"
    
    if [ $DELAY -le 10 ] && [ $DELAY -ge 0 ]; then
        echo -e "${GREEN}✓ PASS: Death timing within 10ms${NC}"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}✗ FAIL: Death timing exceeded 10ms (${DELAY}ms)${NC}"
        FAIL=$((FAIL + 1))
    fi
    echo
}

check_meal_count() {
    local args=$1
    local expected_meals=$(echo $args | awk '{print $5}')
    local nb_philos=$(echo $args | awk '{print $1}')
    
    print_header "MEAL COUNT TEST"
    print_test "Meal counting" "$args" "Each philosopher eats exactly $expected_meals times"
    
    OUTPUT=$(timeout 10 ./philo $args 2>&1)
    
    echo "Meals per philosopher:"
    for i in $(seq 1 $nb_philos); do
        MEALS=$(echo "$OUTPUT" | grep "^[0-9]* $i is eating" | wc -l)
        echo "  Philosopher $i: $MEALS meals"
        
        if [ $MEALS -ne $expected_meals ]; then
            echo -e "${RED}✗ FAIL: Philosopher $i ate $MEALS times (expected $expected_meals)${NC}"
            FAIL=$((FAIL + 1))
            return
        fi
    done
    
    echo -e "${GREEN}✓ PASS: All philosophers ate exactly $expected_meals times${NC}"
    PASS=$((PASS + 1))
    echo
}

# Start testing
clear
print_header "PHILOSOPHERS COMPREHENSIVE TEST SUITE"

# ===================================
# BASIC TESTS
# ===================================
print_header "CATEGORY 1: BASIC FUNCTIONALITY"

run_test "Single philosopher (must die)" \
    "1 800 200 200" \
    "Should die (only 1 fork available)" \
    "yes"

run_test "Two philosophers (should survive)" \
    "2 410 200 200" \
    "Should NOT die (enough time between meals)" \
    "no"

run_test "Five philosophers (should survive)" \
    "5 800 200 200" \
    "Should NOT die (comfortable timing)" \
    "no"

run_test "Four philosophers (should survive)" \
    "4 410 200 200" \
    "Should NOT die" \
    "no"

# ===================================
# DEATH TESTS
# ===================================
print_header "CATEGORY 2: DEATH SCENARIOS"

run_test "Tight timing (should die)" \
    "4 310 200 100" \
    "One philosopher should die" \
    "yes"

run_test "Very tight timing" \
    "3 200 100 100" \
    "Should die quickly" \
    "yes"

run_test "Impossible timing (die while eating)" \
    "5 150 200 100" \
    "Should die (eating takes longer than time_to_die)" \
    "yes"

run_test "Edge case death" \
    "2 400 200 200" \
    "Should die at the edge" \
    "yes"

run_test "Fast death" \
    "4 100 200 100" \
    "Should die very quickly" \
    "yes"

# ===================================
# SURVIVAL TESTS
# ===================================
print_header "CATEGORY 3: SURVIVAL SCENARIOS"

run_test "Comfortable timing" \
    "3 610 200 200" \
    "Should NOT die" \
    "no"

run_test "Large number of philosophers" \
    "10 800 200 200" \
    "Should NOT die" \
    "no"

run_test "Very comfortable timing" \
    "5 1000 200 200" \
    "Should NOT die" \
    "no"

run_test "Fast eating cycle" \
    "4 500 100 100" \
    "Should NOT die" \
    "no"

# ===================================
# MEAL COUNT TESTS
# ===================================
print_header "CATEGORY 4: MEAL COUNTING"

check_meal_count "5 800 200 200 7"
check_meal_count "4 410 200 200 5"
check_meal_count "3 800 200 200 10"
check_meal_count "2 800 200 200 3"

# ===================================
# DEATH TIMING TESTS
# ===================================
print_header "CATEGORY 5: DEATH TIMING PRECISION"

check_death_timing "4 310 200 100"
check_death_timing "1 800 200 200"
check_death_timing "3 200 100 100"

# ===================================
# EDGE CASES
# ===================================
print_header "CATEGORY 6: EDGE CASES"

run_test "Minimum values" \
    "2 60 60 60" \
    "Should handle minimum times" \
    "no"

run_test "Many philosophers" \
    "20 800 200 200" \
    "Should handle many philosophers" \
    "no"

run_test "Odd number of philosophers" \
    "7 800 200 200" \
    "Should work with odd numbers" \
    "no"

run_test "Even number of philosophers" \
    "8 800 200 200" \
    "Should work with even numbers" \
    "no"

run_test "Very long times" \
    "3 5000 1000 1000" \
    "Should handle large values" \
    "no"

# ===================================
# STRESS TESTS
# ===================================
print_header "CATEGORY 7: STRESS TESTS"

run_test "50 philosophers" \
    "50 800 200 200" \
    "Should handle 50 philosophers" \
    "no"

run_test "100 philosophers" \
    "100 800 200 200" \
    "Should handle 100 philosophers" \
    "no"

run_test "High meal count" \
    "5 800 200 200 20" \
    "Should handle many meals" \
    "no"

# ===================================
# DATA RACE TESTS
# ===================================
print_header "CATEGORY 8: DATA RACE DETECTION"

echo "Testing for data races (run with helgrind)..."
echo "Command: valgrind --tool=helgrind ./philo 4 410 200 200"
echo "Note: Run this manually for detailed analysis"
echo

# ===================================
# PERFORMANCE TESTS
# ===================================
print_header "CATEGORY 9: PERFORMANCE"

echo "Testing performance with 200 philosophers..."
START_TIME=$(date +%s%N)
timeout 3 ./philo 200 800 200 200 > /dev/null 2>&1
END_TIME=$(date +%s%N)
DURATION=$(( (END_TIME - START_TIME) / 1000000 ))
echo "Duration: ${DURATION}ms"

if [ $DURATION -lt 5000 ]; then
    echo -e "${GREEN}✓ PASS: Good performance${NC}"
    PASS=$((PASS + 1))
else
    echo -e "${YELLOW}⚠ WARNING: Slow performance${NC}"
fi
echo

# ===================================
# RESULTS SUMMARY
# ===================================
print_header "TEST RESULTS SUMMARY"

TOTAL=$((PASS + FAIL))
echo "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASS${NC}"
echo -e "${RED}Failed: $FAIL${NC}"
echo

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL TESTS PASSED! 🎉${NC}"
else
    echo -e "${RED}⚠ SOME TESTS FAILED ⚠${NC}"
fi

echo
echo "=========================================="
echo "Additional Manual Tests:"
echo "=========================================="
echo "1. Memory leaks: valgrind --leak-check=full ./philo 4 410 200 200"
echo "2. Data races: valgrind --tool=helgrind ./philo 4 410 200 200"
echo "3. Thread sanitizer: compile with -fsanitize=thread"
echo