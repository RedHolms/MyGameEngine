#include "Core/List.hpp"

extern "C" __declspec(dllimport) void __stdcall OutputDebugStringW(const wchar_t*);

#define EXPECT(Expr)                                                                                                   \
  do {                                                                                                                 \
    if (!(Expr)) {                                                                                                     \
      OutputDebugStringW(L"Expression failed: " L ## #Expr L"\n");                                                     \
      return false;                                                                                                    \
    }                                                                                                                  \
  } while (false)

// ------------------------
// Helper non-trivial type
// ------------------------
struct Tracker {
  static inline int ctorCount = 0;
  static inline int dtorCount = 0;
  static inline int copyCount = 0;
  static inline int moveCount = 0;

  int value = 0;

  Tracker(int v = 0) : value(v) {
    ++ctorCount;
  }

  Tracker(const Tracker& other) : value(other.value) {
    ++copyCount;
  }

  Tracker(Tracker&& other) noexcept : value(other.value) {
    ++moveCount;
  }

  ~Tracker() {
    ++dtorCount;
  }

  static void Reset() {
    ctorCount = dtorCount = copyCount = moveCount = 0;
  }
};

// ------------------------
// Basic functionality
// ------------------------
static bool DefaultConstructedIsEmptyTest() {
  List<int> list;

  EXPECT(list.IsEmpty());
  EXPECT(list.GetCount() == 0);
  EXPECT(list.GetCapacity() == 0);

  return true;
}

static bool AppendIncreasesCountTest() {
  List<int> list;

  list.Append(1);
  list.Append(2);

  EXPECT(list.GetCount() == 2);
  EXPECT(list[0] == 1);
  EXPECT(list[1] == 2);

  return true;
}

static bool ReserveIncreasesCapacityTest() {
  List<int> list;

  list.Reserve(100);

  EXPECT(list.GetCapacity() == 100);
  EXPECT(list.GetCount() == 0);

  return true;
}

// ------------------------
// Insert
// ------------------------
static bool InsertMiddleTest() {
  List<int> list;

  list.Append(1);
  list.Append(3);
  list.Insert(1, 2);

  EXPECT(list.GetCount() == 3);
  EXPECT(list[0] == 1);
  EXPECT(list[1] == 2);
  EXPECT(list[2] == 3);

  return true;
}

static bool InsertOutOfBoundsAppendsTest() {
  List<int> list;

  list.Append(1);
  list.Insert(100, 2);

  EXPECT(list.GetCount() == 2);
  EXPECT(list[1] == 2);

  return true;
}

// ------------------------
// Remove
// ------------------------
static bool RemoveSingleTest() {
  List<int> list;

  list.Append(1);
  list.Append(2);
  list.Append(3);

  list.Remove(1);

  EXPECT(list.GetCount() == 2);
  EXPECT(list[0] == 1);
  EXPECT(list[1] == 3);

  return true;
}

static bool RemoveRangeTest() {
  List<int> list;

  for (int i = 0; i < 5; ++i)
    list.Append(i);

  list.Remove(1, 3);

  EXPECT(list.GetCount() == 2);
  EXPECT(list[0] == 0);
  EXPECT(list[1] == 4);

  return true;
}

// ------------------------
// Negative indexing
// ------------------------
static bool NegativeIndexAccessTest() {
  List<int> list;

  list.Append(10);
  list.Append(20);
  list.Append(30);

  EXPECT(list[-1] == 30);
  EXPECT(list[-2] == 20);

  return true;
}

// ------------------------
// Copy semantics
// ------------------------
static bool CopyConstructorTest() {
  List<int> list;
  list.Append(1);
  list.Append(2);

  List<int> copy = list;

  EXPECT(copy.GetCount() == 2);
  EXPECT(copy[0] == 1);
  EXPECT(copy[1] == 2);

  return true;
}

static bool CopyAssignmentTest() {
  List<int> list;
  list.Append(1);
  list.Append(2);

  List<int> copy;
  copy = list;

  EXPECT(copy.GetCount() == 2);
  EXPECT(copy[0] == 1);
  EXPECT(copy[1] == 2);

  return true;
}

// ------------------------
// Move semantics
// ------------------------
static bool MoveConstructorTest() {
  List<int> list;
  list.Append(42);

  List<int> moved = Move(list);

  EXPECT(moved.GetCount() == 1);
  EXPECT(moved[0] == 42);

  EXPECT(list.GetCount() == 0);

  return true;
}

static bool MoveAssignmentTest() {
  List<int> list;
  list.Append(42);

  List<int> moved;
  moved = Move(list);

  EXPECT(moved.GetCount() == 1);
  EXPECT(moved[0] == 42);

  EXPECT(list.GetCount() == 0);

  return true;
}

// ------------------------
// Iterator
// ------------------------
static bool IteratorTraversalTest() {
  List<int> list;

  for (int i = 0; i < 5; ++i)
    list.Append(i);

  int expected = 0;
  for (auto it = list.begin(); it != list.end(); ++it) {
    EXPECT(*it == expected++);
  }

  return true;
}

static bool ConstIteratorTraversalTest() {
  List<int> list;

  for (int i = 0; i < 5; ++i)
    list.Append(i);

  const List<int>& constRef = list;

  int expected = 0;
  for (auto it = constRef.begin(); it != constRef.end(); ++it) {
    EXPECT(*it == expected++);
  }

  return true;
}

// ------------------------
// Non-trivial type tests
// ------------------------
static bool NonTrivialAppendAndDestroyTest() {
  Tracker::Reset();

  {
    List<Tracker> list;

    list.Append(1);
    list.Append(2);
  }

  EXPECT(Tracker::ctorCount >= 2);
  EXPECT(Tracker::dtorCount >= 2);

  return true;
}

static bool NonTrivialInsertTest() {
  Tracker::Reset();

  List<Tracker> list;

  list.Append(1);
  list.Append(3);
  list.Insert(1, 2);

  EXPECT(list.GetCount() == 3);
  EXPECT(list[1].value == 2);

  return true;
}

static bool NonTrivialRemoveCallsDestructorTest() {
  Tracker::Reset();

  {
    List<Tracker> list;

    list.Append(1);
    list.Append(2);
    list.Append(3);

    list.Remove(1);
  }

  EXPECT(Tracker::dtorCount > 0);

  return true;
}

#define RUN_TEST_FUNC(Name)                                                                                            \
  do {                                                                                                                 \
    OutputDebugStringW(L"Running test " L ## #Name L"\n");                                                             \
    if (!Name()) {                                                                                                     \
      OutputDebugStringW(L"Test " L ## #Name L" failed!\n");                                                           \
      return false;                                                                                                    \
    }                                                                                                                  \
  } while (false)

bool TestBody() {
  RUN_TEST_FUNC(DefaultConstructedIsEmptyTest);
  RUN_TEST_FUNC(AppendIncreasesCountTest);
  RUN_TEST_FUNC(ReserveIncreasesCapacityTest);
  RUN_TEST_FUNC(InsertMiddleTest);
  RUN_TEST_FUNC(InsertOutOfBoundsAppendsTest);
  RUN_TEST_FUNC(RemoveSingleTest);
  RUN_TEST_FUNC(RemoveRangeTest);
  RUN_TEST_FUNC(NegativeIndexAccessTest);
  RUN_TEST_FUNC(CopyConstructorTest);
  RUN_TEST_FUNC(CopyAssignmentTest);
  RUN_TEST_FUNC(MoveConstructorTest);
  RUN_TEST_FUNC(MoveAssignmentTest);
  RUN_TEST_FUNC(IteratorTraversalTest);
  RUN_TEST_FUNC(ConstIteratorTraversalTest);
  RUN_TEST_FUNC(NonTrivialAppendAndDestroyTest);
  RUN_TEST_FUNC(NonTrivialInsertTest);
  RUN_TEST_FUNC(NonTrivialRemoveCallsDestructorTest);
  OutputDebugStringW(L"All tests passed");
  return true;
}
